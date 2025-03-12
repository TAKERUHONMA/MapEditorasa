#include "Stage.h"
#include "Engine/Model.h"
#include "Engine/Input.h"
#include "Engine/Camera.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

bool isPointLight = true; // �f�t�H���g�͓_����
bool useLambert = true;

void Stage::InitConstantBuffer()
{
    D3D11_BUFFER_DESC cb;
    cb.ByteWidth = sizeof(CONSTBUFFER_STAGE);
    cb.Usage = D3D11_USAGE_DYNAMIC;
    cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cb.MiscFlags = 0;
    cb.StructureByteStride = 0;
    HRESULT hr;
    hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
    if (FAILED(hr))
    {
        MessageBox(NULL, "�R���X�^���g�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
    }
}

//�R���X�g���N�^
Stage::Stage(GameObject* parent)
    :GameObject(parent, "Stage"), pConstantBuffer_(nullptr)
{
    hModel_ = -1;
    hGround = -1;
    hRoom_ = -1;
    hBunny_ = -1;
}

//�f�X�g���N�^
Stage::~Stage()
{
}

//������
void Stage::Initialize()
{
    hModel_ = Model::Load("Assets\\Ball.fbx");
    hRoom_ = Model::Load("Assets\\room.fbx");
    hGround = Model::Load("Assets\\plane3.fbx");
    hBunny_ = Model::Load("Assets\\Donut_phong.fbx");
    Camera::SetPosition(XMFLOAT3{ 0, 0.8, -2.8 });
    Camera::SetTarget(XMFLOAT3{ 0,0.8,0 });

    InitConstantBuffer();
}

//�X�V
void Stage::Update()
{
    // �����̈ʒu���ړ������鏈���i�ύX�Ȃ��j
    static bool keyPressed = false;
    if (Input::IsKey(DIK_SPACE))
    {
        if (!keyPressed)
        {
            isPointLight = !isPointLight;  // �����^�C�v�̐؂�ւ�
            keyPressed = true;
        }
    }
    else
    {
        keyPressed = false;
    }

    if (Input::IsKey(DIK_L))   // L�L�[��Lambert���˃��f���ɕύX
    {
        useLambert = true;
    }
    if (Input::IsKey(DIK_P))   // P�L�[��Phong���˃��f���ɕύX
    {
        useLambert = false;
    }

    transform_.rotate_.y += 0.5f;
    if (Input::IsKey(DIK_A))
    {
        XMFLOAT4 p = Direct3D::GetLightPos();
        p = { p.x - 0.01f,p.y, p.z,p.w };
        Direct3D::SetLightPos(p);
    }
    if (Input::IsKey(DIK_D))
    {
        XMFLOAT4 p = Direct3D::GetLightPos();
        p = { p.x + 0.01f,p.y, p.z,p.w };
        Direct3D::SetLightPos(p);
    }
    if (Input::IsKey(DIK_W))
    {
        XMFLOAT4 p = Direct3D::GetLightPos();
        p = { p.x,p.y, p.z + 0.01f,p.w };
        Direct3D::SetLightPos(p);
    }
    if (Input::IsKey(DIK_S))
    {
        XMFLOAT4 p = Direct3D::GetLightPos();
        p = { p.x ,p.y, p.z - 0.01f,p.w };
        Direct3D::SetLightPos(p);
    }
    if (Input::IsKey(DIK_UP))
    {
        XMFLOAT4 p = Direct3D::GetLightPos();
        p = { p.x,p.y + 0.01f, p.z,p.w };
        Direct3D::SetLightPos(p);
    }
    if (Input::IsKey(DIK_DOWN))
    {
        XMFLOAT4 p = Direct3D::GetLightPos();
        p = { p.x ,p.y - 0.01f, p.z,p.w };
        Direct3D::SetLightPos(p);
    }

    // �萔�o�b�t�@���X�V
    CONSTBUFFER_STAGE cb = {};
    cb.lightPosition = Direct3D::GetLightPos();
    cb.lightDirection = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
    XMStoreFloat4(&cb.eyePosition, Camera::GetPosition());
    cb.isPointLight = isPointLight ? 1 : 0;

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &cb, sizeof(cb));
    Direct3D::pContext_->Unmap(pConstantBuffer_, 0);

    Direct3D::pContext_->VSSetConstantBuffers(1, 1, &pConstantBuffer_);
    Direct3D::pContext_->PSSetConstantBuffers(1, 1, &pConstantBuffer_);
}

//�`��
void Stage::Draw()
{

    // ������\��
    Transform ltr;
    ltr.position_ = { Direct3D::GetLightPos().x, Direct3D::GetLightPos().y, Direct3D::GetLightPos().z };
    ltr.scale_ = { 0.1f, 0.1f, 0.1f };
    Model::SetTransform(hModel_, ltr);
    Model::Draw(hModel_);

    // �����A�o�j�[�Ȃǂ̕`�揈��
    Transform tr;
    tr.position_ = { 0, 0, 0 };
    Model::SetTransform(hRoom_, tr);
    Model::Draw(hRoom_);

    static Transform tbunny;
    tbunny.scale_ = { 0.25f, 0.25f, 0.25f };
    tbunny.position_ = { 0, 0.5f, 0 };
    tbunny.rotate_.y += 0.1f;
    Model::SetTransform(hBunny_, tbunny);
    Model::Draw(hBunny_);

    ImGui::Text("Rotate:%.3f", tbunny.rotate_.y);
}

//�J��
void Stage::Release()
{
}
