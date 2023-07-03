
/* Sampler State */
sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

sampler PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

/* Raterizer State */
RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{
	FillMode = Solid;
	CullMode = front;
	FrontCounterClockwise = false;
};
