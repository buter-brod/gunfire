uniform sampler2D texture;

// atlas size
uniform vec2 bigRectSize;

// current texture rect size
uniform vec4 smallRect;

// 1=no pixelize (original texture), 2=pixelize two times, etc.
uniform float coeff;

void main() {

 // small texture rect origin on atlas, in texels
 vec2 smallRectOriginAbs = smallRect.xy;
 
 // small texture rect size on atlas, in texels
 vec2 smallRectSizeAbs = smallRect.zw;
 
 // small texture rect origin in 0..1 of atlas
 vec2 smallRectOriginRel = smallRectOriginAbs / bigRectSize;
 
 // how much atlas is larger than texture
 vec2 multiplier = bigRectSize / smallRectSizeAbs;
 
 // how many squares will be formed by pixeliziation
 vec2 squaresCount = smallRectSizeAbs / coeff;
 
 // small texture's uv in 0..1 of small texture
 vec2 uvRel = (gl_TexCoord[0].xy - smallRectOriginRel) * multiplier;
 
 // main pixelization formula: trunc all to squares
 uvRel -= mod(uvRel, 1.0 / squaresCount);
 
 // convert uv back in atlas space
 vec2 uvAbs = (uvRel / multiplier) + smallRectOriginRel;
 
 gl_FragColor = texture2D(texture, uvAbs);
}