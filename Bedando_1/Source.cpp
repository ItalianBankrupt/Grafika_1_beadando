#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#define numVAOs 2
#define numVBOs 2

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLFWwindow* window;

GLfloat korKoordinatak[362 * 2];
GLfloat szakaszKoordinatak[4];
GLboolean	keyboard[512] = { GL_FALSE };
float sugar = 50.0f;
float inc = 1.0f;
float offsetX = 0.0f;
float offsetY = 0.0f;
float incX = 1.0f;
float incY = 0.56f;
float szakaszEltolas = 0.0f;
bool metszes = true;
bool elteritettIrany = false;

bool vanEMetszes(GLfloat korKoordinatak[], GLfloat szakaszKoordinatak[]) {
	float centerX = korKoordinatak[0];
	float centerY = korKoordinatak[1];
	float normSugar = (sugar / 600) * 2;

	float x1 = szakaszKoordinatak[0];
	float y1 = szakaszKoordinatak[1];
	float x2 = szakaszKoordinatak[2];
	float y2 = szakaszKoordinatak[3];

	int generaltPontok = 100;
	for (int i = 0; i <= generaltPontok; ++i) {
		float t = (float)i / (float)generaltPontok;
		float szakaszX = x1 + t * (x2 - x1);
		float szakaszY = y1 + t * (y2 - y1);

		float dx = szakaszX - centerX;
		float dy = szakaszY - centerY;
		float negyzetesTavolsag = dx * dx + dy * dy;

		if (negyzetesTavolsag <= normSugar * normSugar) {
			return true; 
		}
	}
	return false; 
}

void szakaszRajzolas() {
	int kepernyoSzelessege, kepernyoMagassaga;
	glfwGetWindowSize(window, &kepernyoSzelessege, &kepernyoMagassaga);

	float hossz = kepernyoSzelessege / 2.0f;
	float kezdoX = (kepernyoSzelessege - hossz) / 2.0f; 
	float vegsoX = kezdoX + hossz;
	float y = kepernyoMagassaga / 2.0f + szakaszEltolas * kepernyoMagassaga;

	glLineWidth(3.0f);

	szakaszKoordinatak[0] = (2.0f * kezdoX) / kepernyoSzelessege - 1.0f;
	szakaszKoordinatak[2] = (2.0f * vegsoX) / kepernyoSzelessege - 1.0f;
	szakaszKoordinatak[1] = (2.0f * y) / kepernyoMagassaga - 1.0f;
	szakaszKoordinatak[3] = szakaszKoordinatak[1];

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(szakaszKoordinatak), szakaszKoordinatak, GL_STATIC_DRAW);
	GLint posAttrib = glGetAttribLocation(renderingProgram, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(posAttrib);
	glUniform1i(glGetUniformLocation(renderingProgram, "isLine"), 1);
	glUniform1i(glGetUniformLocation(renderingProgram, "moving"), 0);
	GLint eltolAttrib = glGetUniformLocation(renderingProgram, "szakaszEltolas");
	glUniform1f(eltolAttrib, szakaszEltolas);
	glDrawArrays(GL_LINES, 0, 2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void korRajzolas() {
	int kepernyoSzelessege, kepernyoMagassaga;
	glfwGetWindowSize(window, &kepernyoSzelessege, &kepernyoMagassaga);

	float centerX = kepernyoSzelessege / 2.0f + offsetX;
	float centerY = kepernyoMagassaga / 2.0f + offsetY;
	
	float nemTeritettCenterX = 10.0f;
	float nemTeritettCenterY = 0.0f;

	float teritettCenterX = 10.0f * cos(25.0f * 3.14159f / 180.0f);
	float teritettCenterY = 10.0f * sin(25.0f * 3.14159f / 180.0f);


	if (elteritettIrany) {
		centerX += teritettCenterX;
		centerY += teritettCenterY;
	}
	else {
		centerX += nemTeritettCenterX;
		centerY += nemTeritettCenterY;
	}
	
	if (centerX + sugar >= kepernyoSzelessege || centerX - sugar <= 0) {
		incX = -incX;  
	}

	if (centerY + sugar >= kepernyoMagassaga || centerY - sugar <= 0) {
		incY = -incY;
	}

	offsetX += incX;
	if (elteritettIrany == true) {
		offsetY += incY;
	}
	

	korKoordinatak[0] = (2.0f * centerX) / kepernyoSzelessege - 1.0f;
	korKoordinatak[1] = (2.0f * centerY) / kepernyoMagassaga - 1.0f;


	for (int i = 0; i <= 360; ++i) {
		float fok = i * 3.14159f / 180.0f;
		float x = centerX + sugar * cos(fok);
		float y = centerY + sugar * sin(fok);

		korKoordinatak[2 * (i + 1)] = (2.0f * x) / kepernyoSzelessege - 1.0f;
		korKoordinatak[2 * (i + 1) + 1] = (2.0f * y) / kepernyoMagassaga - 1.0f;
	}
	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(korKoordinatak), korKoordinatak, GL_STATIC_DRAW);
	GLint posAttrib = glGetAttribLocation(renderingProgram, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(posAttrib);
	glUniform2f(glGetUniformLocation(renderingProgram, "center"), centerX, centerY);
	glUniform1f(glGetUniformLocation(renderingProgram, "radius"), 50.0f);
	glUniform1i(glGetUniformLocation(renderingProgram, "isLine"), 0);
	glUniform1i(glGetUniformLocation(renderingProgram, "moving"), 1);
	glUniform1i(glGetUniformLocation(renderingProgram, "offsetX"), offsetX);
	glUniform1i(glGetUniformLocation(renderingProgram, "offsetY"), offsetY);
	glUniform1i(glGetUniformLocation(renderingProgram, "intersection"), metszes);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 362);  

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

bool checkOpenGLError() {
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

void printShaderLog(GLuint shader) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		cout << "Program Info Log: " << log << endl;
		free(log);
	}
}

/** Függvény, a shader fájlok sorainak beolvasásához. */
string readShaderSource(const char* filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";

	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

GLuint createShaderProgram() {

	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	/** Beolvassuk a shader fájlok tartalmát. */
	string vertShaderStr = readShaderSource("vertexShader.glsl");
	string fragShaderStr = readShaderSource("fragmentShader.glsl");

	/** Létrehozzuk a shader objektumainkat. */
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	/** A shader fájlok tartalmát eltároló string objektum szöveggé konvertálásás is elvégezzük. */
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	/** Ekkor a betöltött kódot hozzárendelhetjük a shader objektumainkhoz. */
	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	/** Fordítsuk le ezen shader objektumhoz rendelt kódot. */
	glCompileShader(vShader);
	/** Hibakeresési lépések. Például sikeres volt-e a fordítás? Ha nem, mi volt az oka? */
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "vertex compilation failed" << endl;
		printShaderLog(vShader);
	}

	/** A második shader objektumhoz rendelt kódunkat is lefordítjuk. */
	glCompileShader(fShader);
	/** Ismét hibakeresési lépések. Például sikeres volt-e a fordítás? Ha nem, mi volt az oka? */
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "fragment compilation failed" << endl;
		printShaderLog(fShader);
	}

	/** Shader program objektum létrehozása: õ fogja össze a shadereket. Eltároljuk az ID értéket. */
	GLuint vfProgram = glCreateProgram();
	/** Csatoljuk a shadereket az elõzõ lépésben létrehozott objektumhoz. */
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	/** Végül a GLSL fordítónk ellenõrzi, hogy a csatolt shaderek kompatibilisek-e egymással. */
	glLinkProgram(vfProgram);
	/** Ha hiba lépett fel, nézzük meg mi volt ennek az oka. */
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "linking failed" << endl;
		printProgramLog(vfProgram);
	}

	/** Ha minden rendben ment a linkelés során, az objektumok leválaszthatóak a programról. */
	glDeleteShader(vShader);
	glDeleteShader(fShader);

	/** A kész program a visszatérési értékünk. */
	return vfProgram;
}

/** A jelenetünk utáni takarítás. */
void cleanUpScene()
{
	/** Töröljük a vertex array objektumokat. */
	glDeleteVertexArrays(1, vao);
	glDeleteBuffers(1, &vbo[0]); 
	glDeleteBuffers(1, &vbo[1]); 
	/** Töröljük a shader programot. */
	glDeleteProgram(renderingProgram);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/** ESC billentyûre kilépés. */
	/** Exit on ESC key. */
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	/** A billentyûk lenyomásának és felengedésének regisztrálása. Lehetővé teszi gombkombinációk használatát. */
	/** Let's register press and release events for keys. Enables the usage of key combinations. */
	if (action == GLFW_PRESS) {
		keyboard[key] = GL_TRUE;
	}
	else if (action == GLFW_RELEASE) {
		keyboard[key] = GL_FALSE;
	}
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_UP)) {
		szakaszEltolas += 0.01f;
	}
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_DOWN)) {
		szakaszEltolas -= 0.01f;
	}
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_S)) {
		elteritettIrany = !elteritettIrany;
	}
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{

}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}

void init(GLFWwindow* window) {
	/** A rajzoláshoz használt shader programok betöltése. */
	renderingProgram = createShaderProgram();

	/** Vertex Array Objektumok létrehozása. */
	glGenVertexArrays(numVAOs, vao);  // Különböző VAO-kat hozunk létre

	glBindVertexArray(vao[0]);
	glGenBuffers(1, &vbo[0]);  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(szakaszKoordinatak), szakaszKoordinatak, GL_STATIC_DRAW);

	glBindVertexArray(vao[1]);
	glGenBuffers(1, &vbo[1]);  
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(korKoordinatak), korKoordinatak, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void display(GLFWwindow* window, double currentTime) {

	glClearColor(GLclampf(255.0 / 255.0), GLclampf(255.0 / 255.0), GLclampf(51.0 / 255.0), 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	/** Aktiváljuk a shader-program objektumunkat. */
	glUseProgram(renderingProgram);
	
	korRajzolas();
	szakaszRajzolas();
	metszes = vanEMetszes(korKoordinatak, szakaszKoordinatak);
	/** Ha kirajzoltuk a pontunkat, kísérletezzünk a pont méretének növelésével. */
	//glPointSize(30.0);
	/** Ha vonalat rajzolunk ki, ezzel módosíthatjuk a láthatóságot. */
	//glLineWidth(10.0);

	//glDrawArrays(GL_POINTS, 0, 1);
	//glDrawArrays(GL_LINES, 0, 2);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main(void) {

	/** Próbáljuk meg inicializálni a GLFW-t! */
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	/** A kívánt OpenGL verzió (4.3) */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	/** Próbáljuk meg létrehozni az ablakunkat. */
	window = glfwCreateWindow(600, 600, "Drawing a point", NULL, NULL);

	/** Válasszuk ki az ablakunk OpenGL kontextusát, hogy használhassuk. */
	glfwMakeContextCurrent(window);

	/** Billentyûzethez köthetõ események kezelése. */
	glfwSetKeyCallback(window, keyCallback);
	/** A kurzor helyének vizsgálata. */
	//glfwSetCursorPosCallback(window, cursorPosCallback);
	/** Az egér gombjaihoz köthetõ események kezelése. */
	//glfwSetMouseButtonCallback(window, mouseButtonCallback);

	/** Incializáljuk a GLEW-t, hogy elérhetõvé váljanak az OpenGL függvények. */
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	/** Az alkalmazáshoz kapcsolódó elõkészítõ lépések, pl. hozd létre a shader objektumokat. */
	init(window);

	while (!glfwWindowShouldClose(window)) {
		/** a kód, amellyel rajzolni tudunk a GLFWwindow ojektumunkba. */
		display(window, glfwGetTime());
		/** double buffered */
		glfwSwapBuffers(window);
		/** események kezelése az ablakunkkal kapcsolatban, pl. gombnyomás */
		glfwPollEvents();
	}

	/** Töröljük a GLFW ablakot. */
	glfwDestroyWindow(window);
	/** Leállítjuk a GLFW-t */
	glfwTerminate();

	/** Felesleges objektumok törlése. */
	cleanUpScene();

	exit(EXIT_SUCCESS);
}