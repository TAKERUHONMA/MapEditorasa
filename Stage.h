#pragma once
#include "Engine/GameObject.h"

struct CONSTBUFFER_STAGE
{
    XMFLOAT4 lightPosition;
    XMFLOAT4 lightDirection;
    XMFLOAT4 eyePosition;
    int isPointLight;
    float padding[3]; 
};


//◆◆◆を管理するクラス
class Stage : public GameObject
{
    int hModel_;    //モデル番号
    int hRoom_;
    int hGround;
    int hBunny_;
    ID3D11Buffer* pConstantBuffer_;
    void InitConstantBuffer();
public:
    //コンストラクタ
    Stage(GameObject* parent);

    //デストラクタ
    ~Stage();

    //初期化
    void Initialize() override;

    //更新
    void Update() override;

    //描画
    void Draw() override;

    //開放
    void Release() override;
};