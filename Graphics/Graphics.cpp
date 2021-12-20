#include "Graphics.h"

bool Graphics::initialize(HWND hwnd, int width, int height)
{
	if (!initializeDirectX(hwnd, width, height))
	{
		return false;
	}
	return true;
}

bool Graphics::initializeDirectX(HWND hwnd, int width, int height)
{
	// need pointer to video adapter when creating a device. We don't want the default adapter, since it's not the one we always want.
	// We're going to assume we need the adapter with the highest memory is the best one for the job (Hopefully that means the user's GPU)
	std::vector<AdapterData> adapters = AdapterReader::getAdapters();

	return true;
}
