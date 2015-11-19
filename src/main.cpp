#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

const std::string uniforms = "uniform vec3 iResolution;\nuniform float iGlobalTime;\nuniform vec4 iMouse;\nuniform sampler2D iChannel0;\nuniform sampler2D iChannel1;\nuniform sampler2D iChannel2;\nuniform sampler2D iChannel3;\nuniform vec3 iChannelResolution[4];";
const std::string mainMethod = "void main(){\nmainImage(gl_FragColor, gl_FragCoord.xy);\n}";


int width = 640;
int height = 350;




GLuint iResolutionLocation;
GLuint iGlobalTimeLocation;
GLuint iMouseLocation;
GLuint iChannel0Location;
GLuint iChannel1Location;
GLuint iChannel2Location;
GLuint iChannel3Location;
GLuint iChannelResolutionLocation;


int iChannel0Index=0;
int iChannel1Index=0;
int iChannel2Index=0;
int iChannel3Index=0;


int mouseXPos=0;
int mouseYPos=0;
int mouseXClick=0;
int mouseYClick=0;

GLfloat texW[17];
GLfloat texH[17];



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

void setStaticUniforms(){

	glUniform3f(iResolutionLocation, width, height,1);
	glUniform1i(iChannel0Location, iChannel0Index);
	glUniform1i(iChannel1Location, iChannel1Index);
	glUniform1i(iChannel2Location, iChannel2Index);
	glUniform1i(iChannel3Location, iChannel3Index);

	GLfloat data[]{
		texW[iChannel0Index],texH[iChannel0Index],1.0f,
		texW[iChannel1Index],texH[iChannel1Index],1.0f,
		texW[iChannel2Index],texH[iChannel2Index],1.0f,
		texW[iChannel3Index],texH[iChannel3Index],1.0f
	};



	glUniform3fv(iChannelResolutionLocation, 4, data);

}

void setDynamicUniforms(){

	glUniform1f(iGlobalTimeLocation, SDL_GetTicks()/1000.0f);
	glUniform4f(iMouseLocation, mouseXPos, mouseYPos, mouseXClick, mouseYClick);


}


void loadTextures(){
	glEnable(GL_TEXTURE_2D);
	for(int i=0;i<=16;i++){
		std::string filename = "/home/matt/git/shadertoy/textures/tex"+((i<10)?"0"+std::to_string(i):std::to_string(i))+".bmp";
		SDL_Surface* tex = SDL_LoadBMP(filename.c_str());
		if(tex){
			texW[i]=tex->w;
			texH[i]=tex->h;
			if(tex->w&(tex->w-1)){
				std::cerr<<"Warning: width is not a power of two! ("<<filename<<")\n";
			}
			if(tex->h&(tex->h-1)){
				std::cerr<<"Warning: height is not a power of two! ("<<filename<<")\n";
			}
			int bpp = tex->format->BytesPerPixel;
			GLenum texFormat = GL_NONE;
			if(bpp==4){
				if(tex->format->Rmask==0x000000ff){
					texFormat = GL_RGBA;
				}else{
					texFormat = GL_BGRA;
				}
			}else if(bpp==3){
				if(tex->format->Rmask==0x000000ff){
					texFormat = GL_RGB;
				}else{
					texFormat = GL_BGR;
				}
			}else{
				std::cerr<<"Unknown pixel format! ("<<filename<<")\n";
			}
			glActiveTexture(GL_TEXTURE0+i);
			GLuint textureId;
			glGenTextures(1, &textureId);
			glBindTexture(GL_TEXTURE_2D, textureId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			

			glTexImage2D(GL_TEXTURE_2D, 0, bpp, tex->w, tex->h, 0, texFormat, GL_UNSIGNED_BYTE, tex->pixels);
		}else{
			std::cerr<<"Could not open texture! ("<<filename<<")\n";
			std::cerr<<SDL_GetError()<<"\n";
		}
	}
}


char* getCmdOption(char** begin, char** end, const std::string &option){
	char** it = std::find(begin, end, option);
	if(it!=end && ++it!=end){
		return *it;
	}
	return 0;
}

int main(int argc, char* argv[]){

	if(argc<=1){
		std::cerr<<"Usage: "<<argv[0]<<" <vertexShader.glsl> [-w <width>] [-h <height>] [-c{0,1,2,3,4} index]\n";
		return 1;
	}


	char* wArg = getCmdOption(argv, argv+argc, "-w");
	if(wArg){
		width = std::atoi(wArg);
	}

	char* hArg = getCmdOption(argv, argv+argc, "-h");
	if(hArg){
		height = std::atoi(hArg);
	}

	char* c0Arg = getCmdOption(argv, argv+argc, "-c0");
	if(c0Arg){
		iChannel0Index=std::atoi(c0Arg);
	}
	char* c1Arg = getCmdOption(argv, argv+argc, "-c1");
	if(c1Arg){
		iChannel1Index=std::atoi(c1Arg);
	}
	char* c2Arg = getCmdOption(argv, argv+argc, "-c2");
	if(c2Arg){
		iChannel2Index=std::atoi(c2Arg);
	}
	char* c3Arg = getCmdOption(argv, argv+argc, "-c3");
	if(c3Arg){
		iChannel3Index=std::atoi(c3Arg);
	}
	



	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window;
	SDL_GLContext maincontext;

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window = SDL_CreateWindow("Shadertoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL  );

	maincontext = SDL_GL_CreateContext(window);

	glViewport(0,0,width,height);
	
	SDL_GL_SetSwapInterval(1);

	std::cout<<glGetString(GL_VERSION)<<"\n";
	
	std::ifstream shaderInStream(argv[1]);
	std::stringstream buffer;
	buffer << uniforms << shaderInStream.rdbuf()<<mainMethod;
	std::string contents = buffer.str();
	
	GLuint programIndex = compile(contents);
	glUseProgram(programIndex);

	iResolutionLocation = glGetUniformLocation(programIndex, "iResolution");
	iGlobalTimeLocation = glGetUniformLocation(programIndex, "iGlobalTime");
	iMouseLocation = glGetUniformLocation(programIndex, "iMouse");
	iChannel0Location = glGetUniformLocation(programIndex, "iChannel0");
	iChannel1Location = glGetUniformLocation(programIndex, "iChannel1");
	iChannel2Location = glGetUniformLocation(programIndex, "iChannel2");
	iChannel3Location = glGetUniformLocation(programIndex, "iChannel3");
	iChannelResolutionLocation = glGetUniformLocation(programIndex, "iChannelResolution");


	
	loadTextures();
	
	
	setStaticUniforms();
	
	
	unsigned int lastTime=SDL_GetTicks();
	unsigned int deltaCounter=0;
	unsigned int fps=0;
	unsigned int secondCounter=0;

	bool hasQuit=false;
	while(!hasQuit){
		unsigned int now=SDL_GetTicks();
		deltaCounter+=now-lastTime;
		secondCounter+=now-lastTime;
		lastTime=now;

		while(deltaCounter>17){
			deltaCounter-=17;
		}
		
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
		setDynamicUniforms();
		glRects(-1,-1,1,1);
		SDL_GL_SwapWindow(window);
		fps++;
		if(secondCounter>1000){
			std::cout<<fps<<" FPS\n";
			fps=0;
			secondCounter-=1000;
		}
//		SDL_Delay(10);
	}


	SDL_DestroyWindow(window);
	SDL_Quit();
}
