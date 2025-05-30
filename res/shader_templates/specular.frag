vec3 spec_pos = vec3({{ pos.x }}, {{ pos.y }}, {{ pos.z }});
vec3 spec_color = vec3({{ color.r }}, {{ color.g }}, {{ color.b }});

vec3 spec_viewDir = normalize(viewPos - worldPos.xyz);
    float spec_shininess = 128;
    float spec_specularStrength = 0.3;
    vec3 spec_lightDir = normalize(spec_pos - worldPos.xyz);
    vec3 spec_halfwayDir = normalize(spec_lightDir + spec_viewDir);
    float spec = pow(max(dot(g_norm, spec_halfwayDir), 0.0), spec_shininess);
    vec3 pointSpecular = spec_color * spec * spec_specularStrength;
    FragColor.xyz = FragColor.xyz + pointSpecular;