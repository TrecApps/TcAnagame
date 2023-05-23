#include <GL/glew.h>
#include "TColorBrush.h"
#include "TImageBrush.h"
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

CharWithSize::CharWithSize(WCHAR ch, int weight)
{
	this->ch = ch;
	this->weight = weight;
}

int CharWithSize::compare(const CharWithSize& other) const
{
	int ret = ch - other.ch;
	if (!ret)
		ret = weight - other.weight;
	return ret;
}

bool CharWithSize::operator<(const CharWithSize& other) const {
	return compare(other) < 0;
}

UINT Animation::GetMillisecondRefresh()
{
	return millisecondRefresh;
}
UINT Animation::GetMillisecondLength()
{
	return millisecondLength;
}
void Animation::SetMillisecondRefresh(UINT mr)
{
	millisecondRefresh = mr;
}
void Animation::SetMillisecondLength(UINT ml)
{
	millisecondLength = ml;
}
bool Animation::DoReverse()
{
	return reverse;
}
void Animation::DoReverse(bool reverse)
{
	reverse = true;
}


class CaretRunner : public TcAsyncRunner
{
	friend class DrawingBoard;
	TrecPointerSoft<DrawingBoard> self;
protected:
	// Should Return true once it is "done" or no more rounds are needed
	virtual bool RunRound() override
	{
		TrecPointer<DrawingBoard> board = TrecPointerKey::TrecFromSoft<>(self);
		if (!board.Get())
			return true;
		if (board->caret.intercepter.Get())
		{
			board->LockDrawing();
			board->caret.OnDraw = !board->caret.OnDraw;
			board->needsRefresh = true;
			board->UnlockDrawing();
		}
#ifdef _WINDOWS
		Sleep(1000);
#else
		sleep(1000);
#endif
		return false;
	}
public:
	TrecPointer<TObject::TVariable> TObject::TVariable::Clone(void)
	{
		TrecPointer<TVariable> ret = TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, CaretRunner>();
		dynamic_cast<CaretRunner*>(ret.Get())->self = this->self;
		return ret;
	}
};

DrawingBoard::DrawingBoard(GLFWwindow* window)
{
	if (!window) throw 0;
	this->window = window;

	tabSpace = 4;

	cursors.push_back(glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
	cursors.push_back(glfwCreateStandardCursor(GLFW_IBEAM_CURSOR));
	cursors.push_back(glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
	cursors.push_back(glfwCreateStandardCursor(GLFW_HAND_CURSOR));
	cursors.push_back(glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
	cursors.push_back(glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR));


	defaultClearColor.SetColor(L"white");

	int w = 0;
	int h = 0;
	area.top = h;
	area.left = w;
	glfwGetWindowSize(window, &w, &h);

	area.bottom = h;
	area.right = w;
	origArea = area;
	origArea = area;
	ResetProjection();

	shaderType = shader_type::shader_2d;
	glfwMakeContextCurrent(window);

	caret.thickness = 2.0f;
	caret.OnDraw = false;

	needsRefresh = true;
	needsConstantRefresh = false;

	TcInitLock(&drawingThread);
}

void DrawingBoard::ResetProjection()
{
	orthoProjection = glm::ortho(0.0f, static_cast<float>(area.right), 0.0f, static_cast<float>(area.bottom));
}

void DrawingBoard::AddOperation(TrecPointer<GraphicsOp> ops)
{
	TObjectLocker lock(&thread);
	if (ops.Get())
	{
		this->graphicsOperations.push_back(ops);
	}
}

void DrawingBoard::PerformGraphicsOps()
{
	TObjectLocker lock(&thread);
	glfwMakeContextCurrent(window);
	auto drawBoard = TrecPointerKey::TrecFromSoft<>(self);
	for (UINT Rust = 0; Rust < graphicsOperations.Size(); Rust++) {
		graphicsOperations[Rust]->Perform(drawBoard);
	}
	graphicsOperations.RemoveAll();
}

void DrawingBoard::SetFocusObject(TrecPointer<TObject> focusObject)
{
}

UINT DrawingBoard::GetTabSpace()
{
	return tabSpace;
}


bool DrawingBoard::RetrieveFontEntry(const TString& name, std::map<CharWithSize, GLuint>& map)
{
	return charMap.retrieveEntry(name, map);
}

void DrawingBoard::SetFontEntry(const TString& name, const std::map<CharWithSize, GLuint>& map)
{
	charMap.setEntry(name, map);
}

void DrawingBoard::PrepRefresh()
{
	TObjectLocker lock(&thread);
	this->needsRefresh = true;
}

UINT DrawingBoard::PrepConstantRefresh()
{
	TObjectLocker lock(&thread);
	needsConstantRefresh = true;
	for (UINT Rust = 0; Rust < constantRefreshReqs.Size(); Rust++) {
		if (!constantRefreshReqs[Rust]) {
			constantRefreshReqs[Rust] = true;
			return Rust;
		}
	}
	return constantRefreshReqs.push_back(true);
}

void DrawingBoard::ReleaseConstantRefresh(UINT u)
{
	TObjectLocker lock(&thread);
	if (u >= constantRefreshReqs.Size()) return;
		constantRefreshReqs[u] = false;
	for (UINT Rust = 0; Rust < constantRefreshReqs.Size(); Rust++) {
		if (constantRefreshReqs[Rust])
			return;
	}
	needsConstantRefresh = false;
}

void DrawingBoard::SetCursor(ag_mouse_pointer mPointer)
{
	glfwSetCursor(window, cursors[static_cast<UINT>(mPointer)]);
}

void DrawingBoard::NormalizePoint(const TPoint& input, TPoint& output)
{
	auto r = this->area;
	r.bottom /= 2.0f;
	r.right /= 2.0f;

	output.x = (input.x / r.right) - 1.0f;
	output.y = -((input.y / r.bottom) - 1.0f);
}

void DrawingBoard::LockDrawing()
{
	TcLockObject(&drawingThread);
}
void DrawingBoard::UnlockDrawing()
{
	TcUnlockObject(&drawingThread);
}

void DrawingBoard::InitializeCaretRunner()
{
	if (this->caretRunner.Get())
		return;
	TrecPointer<CaretRunner> cRunner = TrecPointerKey::ConvertPointer<TVariable, CaretRunner>(TrecPointerKey::GetNewSelfTrecPointerAlt<TVariable, CaretRunner>());
	cRunner->self = this->self;

	caretRunner = TrecPointerKey::ConvertPointer<CaretRunner, TcAsyncRunner>(cRunner);
	ReturnObject obj;

	caretRunner->Run(obj);
	caretRunner->Pause();

}

void DrawingBoard::DrawCaret()
{
	if (caret.OnDraw && dynamic_cast<TColorBrush*>(caret.brush.Get()))
	{
		//ATLTRACE(L"");
		this->SetShader(TrecPointer<TShader>(), shader_type::shader_2d);

		dynamic_cast<TColorBrush*>(caret.brush.Get())->DrawLine(caret.bottom, caret.top, caret.thickness);
	}
}

void DrawingBoard::SetCaret(TrecPointer<TTextIntercepter> texter, const TPoint& top, const TPoint& bottom, float thickness)
{
	LockDrawing();
	InitializeCaretRunner();
	PauseCaret();

	NormalizePoint(top, caret.top);
	NormalizePoint(bottom, caret.bottom);
	if (!caret.brush.Get())
		caret.brush = GetSolidColorBrush(TColor());

	if (!caret.intercepter.Get())
		caret.intercepter = texter;
	else if (!texter.Get())
		caret.intercepter.Nullify();
	else if (caret.intercepter->GetTarget() != texter->GetTarget());

	if(thickness)
		caret.thickness = thickness;
	UnlockDrawing();
}
void DrawingBoard::SetCaret(TrecPointer<TTextIntercepter> texter, const TPoint& top, const TPoint& bottom, const TColor& color, float thickness)
{
	LockDrawing();
	InitializeCaretRunner();
	PauseCaret();

	NormalizePoint(top, caret.top);
	NormalizePoint(bottom, caret.bottom);
	caret.brush = GetSolidColorBrush(color);

	if (!caret.intercepter.Get())
		caret.intercepter = texter;
	else if (!texter.Get())
		caret.intercepter.Nullify();
	else if (caret.intercepter->GetTarget() != texter->GetTarget());

	if (thickness)
		caret.thickness = thickness;
	UnlockDrawing();
}
void DrawingBoard::ShowCaret()
{
	if (caretRunner.Get())
		caretRunner->Resume();
}
void DrawingBoard::PauseCaret()
{
	if (caretRunner.Get())
		caretRunner->Pause();
}


void DrawingBoard::AddLayer(const RECT_F& ref)
{

	glfwMakeContextCurrent(window);
	if (!layers.Size())
	{
		glEnable(GL_STENCIL_TEST);
	}
	layers.push_back(ref);


	glColorMask(false, false, false, false);
	glDepthMask(false);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glClear(GL_STENCIL_BUFFER_BIT);

	
	RECT_F actRect;
	TColorBrush::NormalizeRect(actRect, ref, TrecPointerKey::TrecFromSoft<>(self));

	glBegin(GL_QUADS);
	glColor4f(0, 0, 0, 1.0f);
	glVertex2d(actRect.left, actRect.top);
	glVertex2d(actRect.right, actRect.top);
	glVertex2d(actRect.right, actRect.bottom);
	glVertex2d(actRect.left, actRect.bottom);
	glEnd();

	glColorMask(true, true, true, true);
	glDepthMask(true);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void DrawingBoard::PopLayer()
{
	if (!layers.Size())
		return;

	glColorMask(false, false, false, false);
	glDepthMask(false);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_DECR, GL_DECR, GL_DECR);

	RECT_F actRect;
	TColorBrush::NormalizeRect(actRect, layers.RemoveAt(layers.Size()-1), TrecPointerKey::TrecFromSoft<>(self));
	glBegin(GL_QUADS);
	glColor4f(0, 0, 0, 1.0f);
	glVertex2d(actRect.left, actRect.top);
	glVertex2d(actRect.right, actRect.top);
	glVertex2d(actRect.right, actRect.bottom);
	glVertex2d(actRect.left, actRect.bottom);
	glEnd();

	glColorMask(true, true, true, true);
	glDepthMask(true);

	if (!layers.Size())
		glDisable(GL_STENCIL_TEST);
}

GLuint DrawingBoard::GetTextureShaderId()
{
	if (!shaderTex2D.Get())
		shaderTex2D = TrecPointerKey::GetNewTrecPointerAlt<TShader, Texture2DShader>();
	return shaderTex2D->GetShaderId();
}

void DrawingBoard::SetSelf(TrecPointer<DrawingBoard> s)
{
	if (this != s.Get())
		throw 1;
	self = TrecPointerKey::SoftFromTrec<>(s);
}

RECT_F DrawingBoard::GetArea()
{
	return area;
}

RECT_F DrawingBoard::GetOrigArea()
{
	return origArea;
}

GLFWwindow* DrawingBoard::GetGlfwWindow()
{
	return window;
}

TrecPointer<TBrush> DrawingBoard::GetHighlightBrush()
{
	if (!textHighlight.Get())
	{
		TColor color(L"lightyellow");
		// To-Do: get actual color from property

		// End To-Do

		textHighlight = GetSolidColorBrush(color);
	}
	return textHighlight;
}

bool DrawingBoard::IsContained(const TPoint& point, const RECT_F& loc)
{

	return (loc.bottom >= loc.top) &&
		(loc.right >= loc.left) &&

		(point.x >= loc.left) &&
		(loc.right >= point.x) &&
		(point.y >= loc.top) &&
		(loc.bottom >= point.y);
}

bool DrawingBoard::IsContained(const TPoint& cp, const ELLIPSE_F& el)
{
	return (pow((cp.x - el.point.x), 2) / pow(el.x, 2)
		+ pow((cp.y - el.point.y), 2) / pow(el.y, 2)) <= 1;
}

bool DrawingBoard::GetDisplayResolution(int& width, int& height)
{
	GLFWmonitor* mon = glfwGetWindowMonitor(window);
	if (!mon)
		mon = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(mon);

	if (!mode)
		return false;
	
	width = mode->width;
	height = mode->height;
	return true;
}

DrawingBoard::~DrawingBoard()
{
	glfwDestroyWindow(window);
	window = nullptr;
	TcRemoveLock(&drawingThread);
}

TrecPointer<TColorBrush> gBrush;

void DrawingBoard::BeginDraw() const
{
	glfwMakeContextCurrent(window);
	glClearColor(
		defaultClearColor.GetRed(),
		defaultClearColor.GetGreen(), 
		defaultClearColor.GetRed(),
		defaultClearColor.GetOpacity());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLoadIdentity(); //Reset the drawing perspective
	if(!gBrush.Get())
		gBrush = TrecPointerKey::ConvertPointer<TBrush, TColorBrush>( this->GetSolidColorBrush(defaultClearColor));
	gBrush->FillRectangle(this->area);
}

void DrawingBoard::ConfirmDraw()
{
	DrawCaret();
	glfwSwapBuffers(window);
	glfwMakeContextCurrent(nullptr);
}

int DrawingBoard::SetShader(TrecPointer<TShader> shader, shader_type shaderType)
{
	if (shader.Get() && currentShader.Get() == shader.Get())
		return currentShader->GetShaderId();
	GLint i = 0;
	switch (shaderType)
	{
	case shader_type::shader_3d:
		assert(shader.Get());
		currentShader = shader;
		break;
	case shader_type::shader_2d:
		if (currentShader.Get())
		{
			glUseProgram(0);
			currentShader.Nullify();
		}
		this->shaderType = shaderType;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



		return -1;
	case shader_type::shader_texture:
		if (!shaderTex2D.Get())
			shaderTex2D = TrecPointerKey::GetNewTrecPointerAlt<TShader, Texture2DShader>();
		currentShader = shaderTex2D;
		break;
	case shader_type::shader_write:
		if (!shaderWrite.Get())
			shaderWrite = TrecPointerKey::GetNewTrecPointerAlt<TShader, TFreeTypeShader>();
		currentShader = shaderWrite;
		currentShader->Use();
		this->shaderType = shaderType;
		i = glGetUniformLocation(shaderWrite->GetShaderId(), "projection");
		glUniformMatrix4fv(i, 1, GL_FALSE,
			glm::value_ptr(orthoProjection));
		return currentShader->GetShaderId();
	}
	this->shaderType = shaderType;
	int ret = -1;
	if (currentShader.Get()) {
		currentShader->Use();
		ret = currentShader->GetShaderId();
	}
	return ret;
}

TrecPointer<TShader> DrawingBoard::GenerateShader(TrecPointer<TFileShell> shaderFile)
{
	return TShader::GenerateShader(shaderFile);
}


TrecPointer<TBrush> DrawingBoard::GetSolidColorBrush(const TColor& color) const
{
	TrecPointer<TColorBrush> ret = TrecPointerKey::GetNewTrecPointer<TColorBrush>();

	ret->colors.push_back(color);
	ret->window = self;
	ret->brushType = brush_type::brush_type_solid;
	return TrecPointerKey::ConvertPointer<TColorBrush, TBrush>(ret);
}

TrecPointer<TBrush> DrawingBoard::GetImageBrush(TrecPointer<TFileShell> file) const
{
	if (!file.Get())
		return TrecPointer<TBrush>();
	TrecPointer<TImageBrush> ret = TrecPointerKey::GetNewTrecPointer<TImageBrush>();

	ret->GenerateImageData(file);
	ret->window = self;
	if (!ret->data)
		return TrecPointer<TBrush>();

	GLenum sourceChannel;

	switch (ret->channelCount)
	{
	case 3:
		sourceChannel = GL_RGB;
		break;
	case 4:
		sourceChannel = GL_RGBA;
		break;
	default:
		return TrecPointer<TBrush>();
	}

	glGenTextures(1, &(ret->textureId));
	glBindTexture(GL_TEXTURE_2D, ret->textureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ret->width, ret->height, 0, sourceChannel, GL_UNSIGNED_BYTE, ret->data);
//	glGenerateMipmap(GL_TEXTURE_2D);
	return TrecPointerKey::ConvertPointer<TImageBrush, TBrush>(ret);
}


TrecPointer<TBrush> DrawingBoard::GetImageBrush(UCHAR* data, UINT width, UINT height, GLenum unum) const
{
	if (!data || !width || !height)
		return TrecPointer<TBrush>();
	TrecPointer<TImageBrush> ret = TrecPointerKey::GetNewTrecPointer<TImageBrush>();
	ret->width = width;
	ret->height = height;
	ret->window = self;
	GLuint tex[] = { 0 };
	glGenTextures(1, tex);
	GLenum er = glGetError();
	ret->textureId = tex[0];
	glBindTexture(GL_TEXTURE_2D, ret->textureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ret->width, ret->height, 0, unum, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return TrecPointerKey::ConvertPointer<TImageBrush, TBrush>(ret);
}