RWTexture2D<float4> output_texture;

[numthreads(16, 16, 1)]
void main(
	int3 dispatch_thread_id : SV_DispatchThreadID,
	int3 group_thread_id : SV_GroupThreadID
	)
{
	int2 uv = int2(dispatch_thread_id.xy);
	int width, height;
	output_texture.GetDimensions(width, height); 

	if(uv.x < width && uv.y < height)
    {
        float4 color = float4(0.0, 0.0, 0.0, 1.0);

        if(group_thread_id.x != 0 && group_thread_id.y != 0)
        {
            color.x = float(uv.x)/(width);
            color.y = float(uv.y)/(width);	
        }
		
		output_texture[dispatch_thread_id.xy] = color;
    }
}