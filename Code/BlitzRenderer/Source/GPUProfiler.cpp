#include "GPUProfiler.h"
#include "GraphicsDriver.h"
#include "Renderer.h"

#include "BlitzSystem/System.h"
#include "BlitzSystem/Application.h"

//extern ID3D11Device * g_pDevice;
//extern ID3D11DeviceContext * g_pContext;
void ErrorPrintf (const char * message, ...);
void DebugPrintf (const char * message, ...);
//float Time ();		// Retrieve time in seconds, using QueryPerformanceCounter or whatever

ImplSingleton(GpuProfiler)

// GpuProfiler implementation

//GpuProfiler g_gpuProfiler;

GpuProfiler::GpuProfiler ()
	: m_iFrameQuery(0)
	, m_iFrameCollect(-1)
	, m_frameCountAvg(0)
	, m_tBeginAvg(0.0f)
	, m_currentTimeStamp(0)
{
	memset(m_apQueryTsDisjoint, 0, sizeof(m_apQueryTsDisjoint));
	memset(m_apQueryTs, 0, sizeof(m_apQueryTs));
	memset(m_adT, 0, sizeof(m_adT));
	memset(m_adTAvg, 0, sizeof(m_adT));
	memset(m_adTTotalAvg, 0, sizeof(m_adT));
}

bool GpuProfiler::Init ()
{
	// Create all the queries we'll need

	D3D11_QUERY_DESC queryDesc = { D3D11_QUERY_TIMESTAMP_DISJOINT, 0 };

	ID3D11Device* g_device = GraphicsDriver::Instance()->GetDevice();

	if (FAILED(g_device->CreateQuery(&queryDesc, &m_apQueryTsDisjoint[0])))
	{
		//ErrorPrintf("Could not create timestamp disjoint query for frame 0!");
		return false;
	}

	if (FAILED(g_device->CreateQuery(&queryDesc, &m_apQueryTsDisjoint[1])))
	{
		//ErrorPrintf("Could not create timestamp disjoint query for frame 1!");
		return false;
	}

	queryDesc.Query = D3D11_QUERY_TIMESTAMP;

	for (GTS gts = GTS_BeginFrame; gts < GTS_Max; gts = GTS(gts + 1))
	{
		if (FAILED(g_device->CreateQuery(&queryDesc, &m_apQueryTs[gts][0])))
		{
			//ErrorPrintf("Could not create start-frame timestamp query for GTS %d, frame 0!", gts);
			return false;
		}

		if (FAILED(g_device->CreateQuery(&queryDesc, &m_apQueryTs[gts][1])))
		{
			//ErrorPrintf("Could not create start-frame timestamp query for GTS %d, frame 1!", gts);
			return false;
		}
	}

	return true;
}

void GpuProfiler::Shutdown ()
{
	if (m_apQueryTsDisjoint[0])
		m_apQueryTsDisjoint[0]->Release();

	if (m_apQueryTsDisjoint[1])
		m_apQueryTsDisjoint[1]->Release();

	for (GTS gts = GTS_BeginFrame; gts < GTS_Max; gts = GTS(gts + 1))
	{
		if (m_apQueryTs[gts][0])
			m_apQueryTs[gts][0]->Release();

		if (m_apQueryTs[gts][1])
			m_apQueryTs[gts][1]->Release();
	}
}

void GpuProfiler::BeginFrame ()
{
	m_currentTimeStamp = GTS_BeginFrame;
	while (!m_skippedIndices.empty())
	{
		m_skippedIndices.pop();
	}

	GraphicsDriver::Instance()->GetDeviceContext()->Begin(m_apQueryTsDisjoint[m_iFrameQuery]);
	Timestamp(GTS_BeginFrame);
}

void GpuProfiler::Timestamp (GTS gts)
{
	// Catch up to the current (for when queries get skipped)
	while (++m_currentTimeStamp < gts)
	{
		m_skippedIndices.push(m_currentTimeStamp);
		GraphicsDriver::Instance()->GetDeviceContext()->End(m_apQueryTs[m_currentTimeStamp][m_iFrameQuery]);
	}

	GraphicsDriver::Instance()->GetDeviceContext()->End(m_apQueryTs[gts][m_iFrameQuery]);
	m_currentTimeStamp = gts;
}

void GpuProfiler::EndFrame ()
{
	Timestamp(GTS_EndFrame);
	GraphicsDriver::Instance()->GetDeviceContext()->End(m_apQueryTsDisjoint[m_iFrameQuery]);

	++m_iFrameQuery &= 1;
}

void GpuProfiler::WaitForDataAndUpdate ()
{
	if (m_iFrameCollect < 0)
	{
		// Haven't run enough frames yet to have data
		m_iFrameCollect = 0;
		return;
	}

	// Wait for data
	while (GraphicsDriver::Instance()->GetDeviceContext()->GetData(m_apQueryTsDisjoint[m_iFrameCollect], NULL, 0, 0) == S_FALSE)
	{
		Sleep(1);
	}

	int iFrame = m_iFrameCollect;
	++m_iFrameCollect &= 1;

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT timestampDisjoint;
	if (GraphicsDriver::Instance()->GetDeviceContext()->GetData(m_apQueryTsDisjoint[iFrame], &timestampDisjoint, sizeof(timestampDisjoint), 0) != S_OK)
	{
		//DebugPrintf("Couldn't retrieve timestamp disjoint query data");
		return;
	}

	//if (timestampDisjoint.Disjoint)
	//{
	//	// Throw out this frame's data
	//	//DebugPrintf("Timestamps disjoint");
	//	OutputDebugString("disjoint");
	//	return;
	//}

	UINT64 timestampPrev;
	if (GraphicsDriver::Instance()->GetDeviceContext()->GetData(m_apQueryTs[GTS_BeginFrame][iFrame], &timestampPrev, sizeof(UINT64), 0) != S_OK)
	{
		//DebugPrintf("Couldn't retrieve timestamp query data for GTS %d", GTS_BeginFrame);
		return;
	}

	for (GTS gts = GTS(GTS_BeginFrame + 1); gts < GTS_Max; gts = GTS(gts + 1))
	{
		UINT64 timestamp;
		HRESULT res = GraphicsDriver::Instance()->GetDeviceContext()->GetData(m_apQueryTs[gts][iFrame], &timestamp, sizeof(UINT64), 0);
		if (res != S_OK)
		{
			//DebugPrintf("Couldn't retrieve timestamp query data for GTS %d", gts);
			return;;
		}

		// Check if query got skipped
		if (m_skippedIndices.size() > 0 && m_skippedIndices.front() == gts)
		{
			m_adT[gts] = 0;
		}
		else
		{
			m_adT[gts] = float(timestamp - timestampPrev) / float(timestampDisjoint.Frequency);
			m_adTTotalAvg[gts] += m_adT[gts];
		}

		timestampPrev = timestamp;
	}

	++m_frameCountAvg;
	if (Application::Instance()->GetSystemTime() > m_tBeginAvg + 0.5f)
	{
		for (GTS gts = GTS_BeginFrame; gts < GTS_Max; gts = GTS(gts + 1))
		{
			m_adTAvg[gts] = m_adTTotalAvg[gts] / m_frameCountAvg;
			m_adTTotalAvg[gts] = 0.0f;
		}

		m_frameCountAvg = 0;
		m_tBeginAvg = S_CAST(float, Application::Instance()->GetSystemTime());
	}
}

void GpuProfiler::DrawQueryText(Renderer* renderer)
{
	float dTDrawTotal = 0.0f;
	for (GTS gts = GTS_BeginFrame; gts < GTS_EndFrame; gts = GTS(gts + 1))
		dTDrawTotal += DtAvg(gts);

	float height = 400.0f;

	std::string str = "DrawTime: " + std::to_string(1000.0f * dTDrawTotal).substr(0, 5) + "ms";
	renderer->DrawString(str.c_str(), 20, height);
	height += 25;

	str = "    ShadowClear:\t" + std::to_string(1000.0f * DtAvg(GTS_ShadowClear)).substr(0, 4) + "ms";
	renderer->DrawString(str.c_str(), 20, height);
	height += 25;

	str = "    ShadowObjects:\t" + std::to_string(1000.0f * DtAvg(GTS_ShadowObjects)).substr(0, 4) + "ms";
	renderer->DrawString(str.c_str(), 20, height);
	height += 25;

	str = "    MainClear:\t" + std::to_string(1000.0f * DtAvg(GTS_MainClear)).substr(0, 4) + "ms";
	renderer->DrawString(str.c_str(), 20, height);
	height += 25;

	str = "    MainObjects:\t" + std::to_string(1000.0f * DtAvg(GTS_MainObjects)).substr(0, 4) + "ms";
	renderer->DrawString(str.c_str(), 20, height);
	height += 25;

	str = "    DeferredLightPass:\t" + std::to_string(1000.0f * DtAvg(GTS_DeferredLightPass)).substr(0, 4) + "ms";
	renderer->DrawString(str.c_str(), 20, height);
	height += 25;

	str = "       VoxelGITotal:\t" + std::to_string(1000.0f * DtAvg(GTS_VoxelGITotal)).substr(0, 4) + "ms";
	renderer->DrawString(str.c_str(), 20, height);
	height += 25;

	str = "    FinalForwardDraw:\t" + std::to_string(1000.0f * DtAvg(GTS_FinalForwardDraw)).substr(0, 4) + "ms";
	renderer->DrawString(str.c_str(), 20, height);
	height += 25;

	str = "GPU frame time: " + std::to_string(1000.0f * (dTDrawTotal + DtAvg(GTS_EndFrame)) ).substr(0, 4) + "ms";
	renderer->DrawString(str.c_str(), 20, height);
	height += 25;

}