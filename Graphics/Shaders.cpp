#include "Shaders.h"
#include<filesystem>

bool VertexShader::initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
    OutputDebugString(std::filesystem::current_path().c_str());
    
    // read shader byte code to blob
    HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());
    if (FAILED(hr))
    {
        std::wstring msg = L"Failed to LOAD shader at path: " + shaderpath;
        ErrorLogger::Log(hr, msg);
        return false;
    }

    //// create vertex shader from the byte code
    hr = device->CreateVertexShader(
        this->shader_buffer->GetBufferPointer(), // byte code
        this->shader_buffer->GetBufferSize(), // byte code length
        NULL, // not using d3d linkage
        this->shader.GetAddressOf()); // shader pointer to populate
    //hr = device->CreateVertexShader(this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
    if (FAILED(hr))
    {
        std::wstring msg = L"Failed to CREATE shader after loading from path: " + shaderpath;
        ErrorLogger::Log(hr, msg);
        return false;
    }

    return true;
}

ID3D11VertexShader* VertexShader::getShader()
{
    return this->shader.Get();
}

ID3D10Blob* VertexShader::getBuffer()
{
    return this->shader_buffer.Get();
}
