#include "Shaders.h"

/* VERTEX SHADER */

bool VertexShader::initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
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
        std::wstring msg = L"Failed to CREATE vertex shader after loading from path: " + shaderpath;
        ErrorLogger::Log(hr, msg);
        return false;
    }

    // Create input layout with above information
    hr = device->CreateInputLayout(
        layoutDesc,	// Input layout
        numElements, // number of elements in the input layout
        this->shader_buffer->GetBufferPointer(), // Shader byte code width input signature (?)
        this->shader_buffer->GetBufferSize(), // Byte code length
        this->inputLayout.GetAddressOf() // pointer to address of input layout
    );
    if (FAILED(hr))
    {
        ErrorLogger::Log("Failed to create vertex input layout");
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

ID3D11InputLayout* VertexShader::getInputLayout()
{
    return inputLayout.Get();
}

/* VERTEX SHADER END */



/* PIXEL SHADER */

bool PixelShader::initialize(mwrl::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
    // read pixel shader byte code to blob
    HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());
    if (FAILED(hr))
    {
        std::wstring msg = L"Failed to LOAD pixel shader at path: " + shaderpath;
        ErrorLogger::Log(hr, msg);
        return false;
    }

    //// create pixel shader from the byte code
    hr = device->CreatePixelShader(
        this->shader_buffer->GetBufferPointer(), // byte code
        this->shader_buffer->GetBufferSize(), // byte code length
        NULL, // not using d3d linkage
        this->shader.GetAddressOf()); // shader pointer to populate
    if (FAILED(hr))
    {
        std::wstring msg = L"Failed to CREATE pixel shader after loading from path: " + shaderpath;
        ErrorLogger::Log(hr, msg);
        return false;
    }

    return true;
}

ID3D11PixelShader* PixelShader::getShader()
{
    return shader.Get();
}

ID3D10Blob* PixelShader::getBuffer()
{
    return shader_buffer.Get();
}

/* PIXEL SHADER END */