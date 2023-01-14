// ------------------------------------
// �Է� Ÿ��
// ------------------------------------
struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXTURE0;
};
// ------------------------------------
// ------------------------------------

float4 main(PixelInputType input) : SV_TARGET
{
	float depthValue;
	float4 color;

	// ���̰� (z��ġ) ��������
	depthValue = input.depthPosition.z / input.depthPosition.w;

	// ���̰����� �ؽ��� �÷� ����
	color = float4(depthValue, depthValue, depthValue, 1.0f);

	return color;
}