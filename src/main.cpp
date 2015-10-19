#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

const std::string uniforms = "uniform vec3 iResolution;\nuniform float iGlobalTime;\nuniform vec4 iMouse;\n";
const std::string mainMethod = "void main(){\nmainImage(gl_FragColor, gl_FragCoord.xy);\n}";


const int width = 640;
const int height = 350;

void printShaderInfoLog(GLuint obj){
	int infoLogLength, charsWritten;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);

	if(infoLogLength>0){
		char infoLog[infoLogLength];
		glGetShaderInfoLog(obj, infoLogLength, &charsWritten, infoLog);
		std::cerr<<infoLog<<"\n";
	}
}

void printProgramInfoLog(GLuint obj){
	int infoLogLength, charsWritten;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);

	if(infoLogLength>0){
		char infoLog[infoLogLength];
		glGetProgramInfoLog(obj, infoLogLength, &charsWritten, infoLog);
		std::cerr<<infoLog<<"\n";
	}
}

int compile(std::string fragmentSource){
	const std::string vertexSource="";

	GLuint fragmentShaderIndex = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentSourceStr = fragmentSource.c_str();
	int fragmentSourceLen = fragmentSource.length();
	glShaderSourceARB(fragmentShaderIndex, 1, &(fragmentSourceStr), &(fragmentSourceLen));
	glCompileShader(fragmentShaderIndex);

	printShaderInfoLog(fragmentShaderIndex);

	GLuint programIndex = glCreateProgram();
	glAttachShader(programIndex, fragmentShaderIndex);
	glLinkProgram(programIndex);

	printProgramInfoLog(programIndex);

	return programIndex;

}

int main(int argc, char* argv[]){
	if(argc<=1){
		std::cerr<<"Usage: "<<argv[0]<<" <vertexShader.glsl>\n";
		return 1;
	}

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window;
	SDL_GLContext maincontext;

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);

	window = SDL_CreateWindow("Hans.", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);

	maincontext = SDL_GL_CreateContext(window);

	glViewport(0,0,width,height);

	std::cout<<glGetString(GL_VERSION)<<"\n";
	
	std::ifstream shaderInStream(argv[1]);
	std::stringstream buffer;
	buffer << uniforms << shaderInStream.rdbuf()<<mainMethod;
	std::string contents = buffer.str();
	
	GLuint programIndex = compile(contents);

	GLuint iResolutionLocation = glGetUniformLocation(programIndex, "iResolution");
	GLuint iGlobalTimeLocation = glGetUniformLocation(programIndex, "iGlobalTime");
	GLuint iMouseLocation = glGetUniformLocation(programIndex, "iMouse");
	
	
	
	bool hasQuit=false;
	int mouseXPos=0;
	int mouseYPos=0;
	int mouseXClick=0;
	int mouseYClick=0;
	while(!hasQuit){
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT:
					hasQuit=true;
					break;
				case SDL_MOUSEBUTTONDOWN:
					mouseXPos=mouseXClick=e.button.x;
					mouseYPos=mouseYClick=e.button.y;
					break;
				case SDL_MOUSEMOTION:
					if(e.motion.state&SDL_BUTTON(1)){
						mouseXPos=e.motion.x;
						mouseYPos=e.motion.y;
					}
					
				

			}
		}
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(programIndex);
		glUniform3f(iResolutionLocation, width, height, 1.0f);
		glUniform1f(iGlobalTimeLocation, SDL_GetTicks()/1000.0f);
		glUniform4f(iMouseLocation, mouseXPos, mouseYPos, mouseXClick, mouseYClick);
		glBegin(GL_TRIANGLE_STRIP);
		//glColor3f(1.0f,1.0f,1.0f);
		glVertex2f(-1.0f,1.0f);
		glVertex2d(-1.0f,-1.0f);
		glVertex2d(1.0f,1.0f);
		glVertex2d(1.0f,-1.0f);
		glEnd();
		
		SDL_GL_SwapWindow(window);
	}


	SDL_DestroyWindow(window);
	SDL_Quit();
}
