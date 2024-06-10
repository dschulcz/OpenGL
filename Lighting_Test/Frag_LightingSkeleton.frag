#version 430

// pipeline-ból bejövő per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// kimenő érték - a fragment színe
out vec4 fs_out_col;

// textúra mintavételező objektum
uniform sampler2D texImage;

 uniform vec3 cameraPos;

// fenyforras tulajdonsagok
// uniform vec4 lightPos = vec4( 0.0, 1.0, 0.0, 0.0);

 uniform vec3 La = vec3(0.0, 0.0, 0.2 );
 uniform vec3 Ld = vec3(1.0, 1.0, 1.0 );
 uniform vec3 Ls = vec3(1.0, 1.0, 1.0 );

// uniform float lightConstantAttenuation    = 1.0;
// uniform float lightLinearAttenuation      = 0.0;
// uniform float lightQuadraticAttenuation   = 0.0;

// anyag tulajdonsagok

 uniform vec3 Ka = vec3( 1.0 );
 uniform vec3 Kd = vec3( 1.0 );
 uniform vec3 Ks = vec3( 1.0 );

// uniform float Shininess = 1.0;

/* segítség:
	    - normalizálás: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - skaláris szorzat: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
				reflect(beérkező_vektor, normálvektor);
		- pow: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
				pow(alap, kitevő);
*/

uniform vec3 light_dir = normalize(vec3(-1,-1,-1));
uniform float shininess = 50;

uniform vec3 light_pos = vec3(20,20,20);
uniform vec3 La_point = vec3(0.0, 0.0, 0.2 );
 uniform vec3 Ld_point = vec3(1.0, 1.0, 1.0 );
 uniform vec3 Ls_point = vec3(1.0, 1.0, 1.0 );
 uniform float shininess_point = 50;
 uniform float point_light_strength = 50;
 uniform vec3 coeffs = vec3(1,1,1);
 uniform float alpha = 1.0f;

void main()
{
	//fs_out_col = vec4(normalize(vs_out_norm),1); // normálvektorok
	
	vec3 n = normalize(vs_out_norm);
	vec3 c = texture(texImage, vs_out_tex).xyz;
	vec3 to_camera = normalize(cameraPos - vs_out_pos);

	// Irány fényforrás
	float diffuse = clamp(dot(n, -light_dir),0,1);
	
	vec3 reflected_dir = reflect(light_dir, n);
	float specular = pow(clamp(dot(reflected_dir, to_camera),0,1),shininess);

	vec3 direcional_light = c * La * Ka + c * diffuse * Ld * Kd + c * specular * Ks * Ls;

	// Pont fényforrás
	vec3 from_point_light = normalize(vs_out_pos - light_pos);
	diffuse = clamp(dot(n, -from_point_light),0,1);
	
	reflected_dir = reflect(from_point_light, n);
	specular = pow(clamp(dot(reflected_dir, to_camera),0,1),shininess_point);

	float d = distance(light_pos, vs_out_pos);

	vec3 point_light = c * La_point * Ka + c * diffuse * Ld_point * Kd + c * specular * Ls_point * Ks;
//	point_light *= point_light_strength;
	float attenuation = coeffs.x * d*d + coeffs.y * d + coeffs.z;
	point_light /= attenuation;

	fs_out_col =vec4(direcional_light + point_light,alpha);
}