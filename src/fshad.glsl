void main()
{
    // Set fragment color from texture
    gl_FragColor = texture2D(texture, v_texcoord);
}
