Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);

SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);


// --------------------------------------
// ��� ���� Ÿ��
// --------------------------------------
cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
};
// --------------------------------------
// --------------------------------------

// --------------------------------------
// �ȼ� ���̴� �Է� Ÿ��
// --------------------------------------
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};
// --------------------------------------
// --------------------------------------

float4 main(PixelInputType input) : SV_TARGET
{
	float bias;
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;


	// ���е��� ���� bias�� ����
	bias = 0.00001f;

	// ambient ������ �⺻��������
	color = ambientColor;

	// �� �������� �������� ���� �ش� ������ �ؽ��� ��ǥ
	projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	// 0 ~ 1 ������ �ִ���
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// �� �������� �������� ���� �ش� ������ �ؽ��� ��ǥ���� �������� �� ����(�ش� �ȼ��� rgb���� ���� ����)
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;

		// ���� ���� ���
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// ���е� ���
		lightDepthValue = lightDepthValue - bias;

		// depthValue(���� �ؽ��Ŀ��� ���� ���� ����)�� lightDepthValue(�ش� ������ �����κ����� �Ÿ�)���� ũ�ٸ�
		// �׷��� (ũ�ٴ°� ���� �ٶ���� �� �ڱ� �տ� �ƹ��͵� ���ٴ� ��)
		// �׷����� �ʴ´ٸ� �⺻ ambientColor�� �Ǿ� ���� ���� -> �׸��ڶ�� ��)
		if (lightDepthValue < depthValue)
		{
			// ���� �� ���
			lightIntensity = saturate(dot(input.normal, input.lightPos));

			if (lightIntensity > 0.0f)
			{
				// Ȯ�� ����� ���� ���� �̿��� color ���
				color += (diffuseColor * lightIntensity);

				// ���� ���� ����
				color = saturate(color);
			}
		}
	}

	// �ڱ��� �ؽ��� ��ǥ �̿��� �ؽ��� ���ø�
	textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);

	// ���� �ؽ��� ���� ����
	color = color * textureColor;

	return color;
}