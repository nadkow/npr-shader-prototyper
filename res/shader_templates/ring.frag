    //get the dot product between the normal and the view direction
    vec3 ring_viewDir = normalize(viewPos - vec3(worldPos));
    float ring_dotproduct = max(dot(Normal, ring_viewDir), .3);
    float ring_value = sin({{ bands }}/ring_dotproduct) * .5 + .5;
    FragColor.xyz = vec3(ring_value, ring_value, ring_value);