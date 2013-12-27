layout (points) in;
layout (points) out;
layout (max_vertices = 1) out;

in vData
{
    vec3 normal;
    vec4 color;
}vertices[];

out fData
{
    vec3 normal;
    vec4 color;
}frag;

void main()
{
    int i;
    for(i = 0;i < gl_in.length();i++)// gl_in.length() = 1 though!
    {
        frag.normal = vertices[i].normal;
        frag.color = vertices[i].color;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
