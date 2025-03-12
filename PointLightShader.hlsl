//������������������������������������������������������������������������������
 // �e�N�X�`�����T���v���[�f�[�^�̃O���[�o���ϐ���`
//������������������������������������������������������������������������������
Texture2D g_texture : register(t0); //�e�N�X�`���[
SamplerState g_sampler : register(s0); //�T���v���[

//������������������������������������������������������������������������������
// �R���X�^���g�o�b�t�@
// DirectX �����瑗�M����Ă���A�|���S�����_�ȊO�̏����̒�`
//������������������������������������������������������������������������������
cbuffer gModel : register(b0)
{
    float4x4 matWVP; // ���[���h�E�r���[�E�v���W�F�N�V�����̍����s��
    float4x4 matW; // ���[���h�ϊ��}�g���N�X
    float4x4 matNormal; // ���[���h�s��
    float4 diffuseColor; // �}�e���A���̐F���g�U���ˌW��
    float4 factor;
    float4 ambientColor;
    float4 specularColor;
    float4 shininess;

    bool isTextured; // �e�N�X�`���[���\���Ă��邩�ǂ���
    bool useLambert; // Lambert���˃��f�����g�p���邩�itrue: Lambert, false: Phong�j
};

cbuffer gStage : register(b1)
{
    float4 lightPosition; // �_�����̈ʒu
    float4 lightDirection; // ���s�����̕���
    float4 eyePosition; // �J�����̈ʒu
    bool isPointLight; // true �� �_����, false �� ���s����
};


//������������������������������������������������������������������������������
// ���_�V�F�[�_�[�o�́��s�N�Z���V�F�[�_�[���̓f�[�^�\����
//������������������������������������������������������������������������������
struct VS_OUT
{
    float4 wpos : POSITION0; //�ʒu
    float4 pos : SV_POSITION; //�ʒu
    float2 uv : TEXCOORD; //UV���W
    float4 normal : NORMAL;
    float4 eyev : POSITION1;
    //float4 col : COLOR;
};

//������������������������������������������������������������������������������
// ���_�V�F�[�_
//������������������������������������������������������������������������������
VS_OUT VS(float4 pos : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
	//�s�N�Z���V�F�[�_�[�֓n�����
    VS_OUT outData;

	//���[�J�����W�ɁA���[���h�E�r���[�E�v���W�F�N�V�����s���������
	//�X�N���[�����W�ɕϊ����A�s�N�Z���V�F�[�_�[��
    float4 spos = mul(pos, matWVP);
    float4 wpos = mul(pos, matW);//���[���h���W�ɕϊ�
    float4 wnormal = mul(normal, matNormal);
    
    outData.pos = spos;
    outData.wpos = wpos;
    outData.uv = uv.xy;
    outData.normal = wnormal;
    outData.eyev = eyePosition - wpos;
    
    //float4 dir = normalize(lightPosition - wpos);
    //outData.col = clamp(dot(normalize(wnormal), dir), 0, 1);
    
	//�܂Ƃ߂ďo��
    return outData;
}

//������������������������������������������������������������������������������
// �s�N�Z���V�F�[�_
//������������������������������������������������������������������������������
float4 PS(VS_OUT inData) : SV_Target
{
    float4 diffuse;
    float4 ambient;
    float4 ambentSource = { 0.2, 0.2, 0.2, 1.0 };
    
    float color;
    float4 specular = float4(0, 0, 0, 0);

    if (isPointLight)
    {
        // ������ �_�����̏ꍇ ������
        float3 dir = normalize(lightPosition.xyz - inData.wpos.xyz);

        // ���������i�_�������L�j
        float3 k = { 0.2f, 0.2f, 1.0f };
        float len = length(lightPosition.xyz - inData.wpos.xyz);
        float dTerm = 1.0 / (k.x + k.y * len + k.z * len * len);

        color = saturate(dot(normalize(inData.normal.xyz), dir)) * dTerm;

        // ���ʔ��ˁi�X�y�L�����j
        float3 R = reflect(-dir, normalize(inData.normal.xyz));
        specular = pow(saturate(dot(R, normalize(inData.eyev.xyz))), shininess.x) * specularColor;
    }
    else
    {
        // ������ ���s�����̏ꍇ ������
        float3 dir = normalize(lightDirection.xyz);

        color = saturate(dot(normalize(inData.normal.xyz), dir));

        // ���ʔ��ˁi�X�y�L�����j
        float3 reflectDir = reflect(-dir, normalize(inData.normal.xyz));
        float3 viewDir = normalize(inData.eyev.xyz);
        float spec = pow(saturate(dot(reflectDir, viewDir)), shininess.x);
        specular = spec * specularColor;
    }

    // �e�N�X�`���[�̗L���ŏ����𕪂���
    if (isTextured == false)
    {
        diffuse = diffuseColor * color * factor.x;
        ambient = diffuseColor * ambentSource;
    }
    else
    {
        diffuse = g_texture.Sample(g_sampler, inData.uv) * color * factor.x;
        ambient = g_texture.Sample(g_sampler, inData.uv) * ambentSource;
    }

    // �ŏI�F = �g�U�� + ���ʔ��� + ����
    return diffuse + specular + ambient;
}
