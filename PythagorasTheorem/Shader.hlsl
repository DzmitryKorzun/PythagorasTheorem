struct PsInput
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

float4 VSMain(float4 pos : POSITION, float4 color : COLOR)
{
    PsInput result;
    
    result.position = pos;
    result.color = color;
    
    return result;
}

float4 PSMain(PsInput input): SV_Target
{
    return input.color;
}