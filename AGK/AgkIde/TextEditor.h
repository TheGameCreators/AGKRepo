#pragma once

#include <string>
#include <vector>
#include <set>
#include <array>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <regex>
#include "imgui/imgui.h"

#define MAXPRECHECKLINES 150000

class TextEditor
{
public:
	enum class PaletteIndex
	{
		Default,
		Keyword,
		Number,
		String,
		CharLiteral,
		Punctuation,
		Preprocessor,
		Identifier,
		KnownIdentifier,
		PreprocIdentifier,
		Comment,
		MultiLineComment,
		Background,
		Cursor,
		Selection,
		ErrorMarker,
		Breakpoint,
		LineNumber,
		CurrentLineFill,
		CurrentLineFillInactive,
		CurrentLineEdge,
		BackgroundLine,
		Max
	};

	enum class SelectionMode
	{
		Normal,
		Word,
		Line
	};



	struct SymbolsValues
	{
		int iSymbolType; // 1 = functions.
		char cCommand[MAX_PATH + 1];
		char cParameters[MAX_PATH + 1];
		char cType[MAX_PATH + 1];
		int lineno;
		bool bVisible = true;
		TextEditor * m_InsideEditor;
	};

	struct Breakpoint
	{
		int mLine;
		bool mEnabled;
		std::string mCondition;

		Breakpoint()
			: mLine(-1)
			, mEnabled(false)
		{}
	};
	
	struct Coordinates
	{
		int mLine, mColumn;
		Coordinates() : mLine(0), mColumn(0) {}
		Coordinates(int aLine, int aColumn) : mLine(aLine), mColumn(aColumn)
		{
			//assert(aLine >= 0);
			//assert(aColumn >= 0);
			//MAC after sleep : Assertion failed: (aColumn >= 0), function Coordinates, file /Users/paulj/Documents/SVN/AGKTrunk/AgkIde/TextEditor.h, line 85. ???
			//Should not react on assert , did we do a debug version as release ?
			if (aColumn < 0) aColumn = 0;
			if (aLine < 0) aLine = 0;
		}
		static Coordinates Invalid() { static Coordinates invalid(-1, -1); return invalid; }

		bool operator ==(const Coordinates& o) const
		{
			return
				mLine == o.mLine &&
				mColumn == o.mColumn;
		}

		bool operator !=(const Coordinates& o) const
		{
			return
				mLine != o.mLine ||
				mColumn != o.mColumn;
		}

		bool operator <(const Coordinates& o) const
		{
			if (mLine != o.mLine)
				return mLine < o.mLine;
			return mColumn < o.mColumn;
		}

		bool operator >(const Coordinates& o) const
		{
			if (mLine != o.mLine)
				return mLine > o.mLine;
			return mColumn > o.mColumn;
		}

		bool operator <=(const Coordinates& o) const
		{
			if (mLine != o.mLine)
				return mLine < o.mLine;
			return mColumn <= o.mColumn;
		}

		bool operator >=(const Coordinates& o) const
		{
			if (mLine != o.mLine)
				return mLine > o.mLine;
			return mColumn >= o.mColumn;
		}
	};

	struct Identifier
	{
		Coordinates mLocation;
		std::string mDeclaration;
	};
	
	typedef std::string String;
	typedef std::unordered_map<std::string, Identifier> Identifiers;
	typedef std::unordered_set<std::string> Keywords;
	typedef std::map<int, std::string> ErrorMarkers;
	typedef std::unordered_set<int> Breakpoints;
	typedef std::set<int> Bookmarks;
	typedef std::array<ImU32, (unsigned)PaletteIndex::Max> Palette;
	typedef char Char;

	struct Glyph
	{
		Char mChar;
		PaletteIndex mColorIndex = PaletteIndex::Default;
		bool mMultiLineComment : 1;
		int mInt = 0;
		Glyph(Char aChar, PaletteIndex aColorIndex) : mChar(aChar), mColorIndex(aColorIndex), mMultiLineComment(false),mInt(0) {}
		Glyph(Char aChar, PaletteIndex aColorIndex, bool aMultiLineComment, int aInt) : mChar(aChar), mColorIndex(aColorIndex), mMultiLineComment(aMultiLineComment), mInt(aInt) {}
	};

	typedef std::vector<Glyph> Line;
	typedef std::vector<Line> Lines;

	struct LanguageDefinition
	{
		typedef std::pair<std::string, PaletteIndex> TokenRegexString;
		typedef std::vector<TokenRegexString> TokenRegexStrings;
		typedef bool (*TokenizeCallback)(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end, PaletteIndex & paletteIndex);

		std::string mName;
		Keywords mKeywords;
		Identifiers mIdentifiers;
		Identifiers mPreprocIdentifiers;
		std::string mCommentStart, mCommentEnd;
		bool mAutoIndentation;

		TokenizeCallback mTokenize;

		TokenRegexStrings mTokenRegexStrings;

		bool mCaseSensitive;
		
		LanguageDefinition()
			: mTokenize(nullptr)
		{
		}
		
		static const LanguageDefinition& CPlusPlus();
		static const LanguageDefinition& HLSL();
		static const LanguageDefinition& GLSL();
		static const LanguageDefinition& C();
		static const LanguageDefinition& SQL();
		static const LanguageDefinition& AngelScript();
		static const LanguageDefinition& Lua();
		static const LanguageDefinition& AGK();
	};

	bool mScrollToCursor;
	int mScrollToCursorAddLines = 0;
	bool checkingcomments = false;
	int maxchecklines = 0;
	int maxlinesperSync = 4000;
	bool withinString;
	bool withinTypeDefination;
	char cTypeName[MAX_PATH];
	int iTypeLineNo;
	bool quiteditor = false;
	bool closenotclicked = true;
	bool bEditorVisible = true;
	bool filechanged = false;
	bool dragdropactive = false;
	bool display_linenumbers = true;
	bool saveonreturn = false;
	bool firstrun = false;
	bool resizewindownext = false;
	bool isAGKsource = false;
	int executeLine = -1;
	TextEditor::Breakpoints bpts;
	TextEditor::Bookmarks bmarks;
	TextEditor::Coordinates commentStart;
	int mCurrentMark = 0;
	int m_RenderCount;
	int m_iNewLineNo[MAXPRECHECKLINES];
	int m_iHiddenLineNo[MAXPRECHECKLINES];
	bool m_bLineVisible[MAXPRECHECKLINES];
	bool m_bFoldingActive[MAXPRECHECKLINES];
	bool m_bFoldingPossible[MAXPRECHECKLINES];
	bool m_bFoldingStartWord[MAXPRECHECKLINES];
	bool m_bFoldingEndWord[MAXPRECHECKLINES];
	bool m_bCheckLine[MAXPRECHECKLINES];
	bool m_bFirstCheck[MAXPRECHECKLINES];
	//bool m_bCheckSymbold[MAXPRECHECKLINES];
	ErrorMarkers editor_markers;
	int activeSymbolslist = 0;
	int updateSymbolslist = 1;
	int activeCodePropertieslist = 0;
	int updateCodePropertieslist = 1;
	int activeFunctions = 0, updateFunctions = 0;
	int activeVariables = 0, updateVariables = 0;
	int activeConstants = 0, updateConstants = 0;
	int activeTypes = 0, updateTypes = 0;
	bool bUpdateSymbolsFirstTime = true;
	int iSkibUpdateSymbols = 0;
	//PE: We need to make it a multimap later.
	#define SYMBOLS_MAP std::map<std::string, TextEditor::SymbolsValues>
	SYMBOLS_MAP symbolsList[2];
	std::map<std::int32_t, TextEditor::SymbolsValues> CodeProperties[2];
	bool bFindNextEndOfLine = false;
	bool bSearchActive = false;
	int bUndoAll = 0;
	char cnSearch[MAX_PATH];
	char cnSearchReplace[MAX_PATH];
	bool bMouseToWordActive = false;
	bool bContextActive = false;
	std::string idUnderMouse;
	bool bFreezeUnderMouse = false;

	Breakpoints mBreakpoints;
	Bookmarks mBookMarks;
	Breakpoints mFoldingActive;

	Coordinates mousehoverstart;
	Coordinates mousehoverend;
	float mousehovertime = 0;
	std::string mousehoverstring;
	bool mousehoversearch = true;


	bool bInFolding = false;
	bool bFoldNow = false;
	bool breakonnext = false;
	int iFoldeStarters = 0;
	char cFoldingEnd[80] = { 0 };
	int iFoldingLines = 0;
	int iSpeedColorize = 0;
	int iHiddenLines = 0;
	int iHiddenLinesNew = 0;

	int iFoldingCheckStart = 0;
	int iFoldingCheckCount = 0;

	int iNextFindDialogAction = -1;
	bool selectedfrommouse = false;

	TextEditor();
	~TextEditor();

	void SetLanguageDefinition(const LanguageDefinition& aLanguageDef);
	const LanguageDefinition& GetLanguageDefinition() const { return mLanguageDefinition; }

	//const Palette& GetPalette() const { return mPalette; }
	void SetPalette(const Palette& aValue);

	void SetErrorMarkers(const ErrorMarkers& aMarkers) { mErrorMarkers = aMarkers; }
	void SetBreakpoints(const Breakpoints& aMarkers) { mBreakpoints = aMarkers; }
	void SetBookMarks(const Bookmarks& aMarkers) { mBookMarks = aMarkers; }
	void ToggleSeachReplace(bool change = true,bool forceadd = false);
	void Help(void);
	void BrowserHelp(void);
	void Render(const char* aTitle, const ImVec2& aSize = ImVec2(), bool aBorder = false,bool modalopen = false,char *projectName = NULL);
	void SetText(const std::string& aText);
	void FindNext(void);
	void ToggleLineComments(void);
	void FindPrev(void);
	int WordCount(void);
	void Replace(void);
	void OpenFind(void);
	void SetTextLines(const std::vector<std::string>& aLines);
	std::string GetText() const;
	std::vector<std::string> GetTextLines() const;
	std::string GetSelectedText() const;
	std::string GetCurrentLineText()const;
	
	int GetTotalLines() const { return (int)mLines.size(); }
	bool IsOverwrite() const { return mOverwrite; }

	void SetReadOnly(bool aValue);
	bool IsReadOnly() const { return mReadOnly; }
	bool IsTextChanged() const { return mTextChanged; }
	bool IsCursorPositionChanged() const { return mCursorPositionChanged; }

	Coordinates GetCursorPosition() const { return GetActualCursorCoordinates(); }
	void SetCursorPosition(const Coordinates& aPosition);
	void EnsureCursorVisible(void);

	void InsertText(const std::string& aValue);
	void InsertText(const char* aValue);

	void MoveUp(int aAmount = 1, bool aSelect = false);
	void MoveDown(int aAmount = 1, bool aSelect = false);
	void MoveLeft(int aAmount = 1, bool aSelect = false, bool aWordMode = false);
	void MoveRight(int aAmount = 1, bool aSelect = false, bool aWordMode = false);
	void MoveTop(bool aSelect = false);
	void MoveBottom(bool aSelect = false);
	void MoveHome(bool aSelect = false);
	void MoveEnd(bool aSelect = false);

	void SetSelectionStart(const Coordinates& aPosition);
	void SetSelectionEnd(const Coordinates& aPosition);
	void SetSelection(const Coordinates& aStart, const Coordinates& aEnd, SelectionMode aMode = SelectionMode::Normal,bool MouseSelected = false);
	void SelectWordUnderCursor();
	void SelectAll();
	bool HasSelection() const;

	void Copy();
	void Cut();
	void Paste();
	void PasteString( char * string );
	void DirectReplace(char * string, char * replace, int lineno);
	void InsertTextDirectly(const char *text);
	void Delete();

	bool CanUndo() const;
	bool CanRedo() const;
	void Undo(int aSteps = 1);
	void Redo(int aSteps = 1);

	static const Palette& GetAGKPalette();
	static const Palette& GetDarkPalette();
	static const Palette& GetLightPalette();
	static const Palette& GetRetroBluePalette();
	static const Palette& GetCustomPalette();

	void ColorizeInternal(int lineNo, int lineMax);

	struct EditorState
	{
		Coordinates mSelectionStart;
		Coordinates mSelectionEnd;
		Coordinates mCursorPosition;
	};
	EditorState mState;

private:
	typedef std::vector<std::pair<std::regex, PaletteIndex>> RegexList;


	class UndoRecord
	{
	public:
		UndoRecord() {}
		~UndoRecord() {}

		UndoRecord(
			const std::string& aAdded,
			const TextEditor::Coordinates aAddedStart, 
			const TextEditor::Coordinates aAddedEnd, 
			
			const std::string& aRemoved, 
			const TextEditor::Coordinates aRemovedStart,
			const TextEditor::Coordinates aRemovedEnd,
			
			TextEditor::EditorState& aBefore, 
			TextEditor::EditorState& aAfter);

		void Undo(TextEditor* aEditor);
		void Redo(TextEditor* aEditor);

		std::string mAdded;
		Coordinates mAddedStart;
		Coordinates mAddedEnd;

		std::string mRemoved;
		Coordinates mRemovedStart;
		Coordinates mRemovedEnd;

		EditorState mBefore;
		EditorState mAfter;
	};

	typedef std::vector<UndoRecord> UndoBuffer;

	void ProcessInputs();
	void Colorize(int aFromLine = 0, int aCount = -1);
	void ColorizeRange(int aFromLine = 0, int aToLine = 0);
	void FastColorizeRange(int aFromLine, int aToLine);
	float TextDistanceToLineStart(const Coordinates& aFrom) const;
	int GetPageSize() const;
	int AppendBuffer(std::string& aBuffer, char chr, int aIndex);
	std::string GetText(const Coordinates& aStart, const Coordinates& aEnd) const;
	Coordinates GetActualCursorCoordinates() const;
	Coordinates SanitizeCoordinates(const Coordinates& aValue) const;
	void Advance(Coordinates& aCoordinates) const;
	void DeleteRange(const Coordinates& aStart, const Coordinates& aEnd);
	int InsertTextAt(Coordinates& aWhere, const char* aValue);
	void AddUndo(UndoRecord& aValue);
	Coordinates ScreenPosToCoordinates(const ImVec2& aPosition, bool precision) const;
	Coordinates FindWordStart(const Coordinates& aFrom) const;
	Coordinates FindWordEnd(const Coordinates& aFrom) const;
	bool IsOnWordBoundary(const Coordinates& aAt) const;
	void RemoveLine(int aStart, int aEnd);
	void RemoveLine(int aIndex);
	Line& InsertLine(int aIndex);
	void EnterCharacter(Char aChar, bool aShift, int codepoint = 0);
	void BackSpace();
	void DeleteSelection();
	std::string GetWordUnderCursor() const;
	std::string GetWordAt(const Coordinates& aCoords,bool variable=false,bool includefunctions = false) const;

	float mLineSpacing;
	Lines mLines;

	UndoBuffer mUndoBuffer;
	int mUndoIndex;
	int iColorLine = 0;
	
	int mTabSize;
	bool mOverwrite;
	bool mReadOnly;
	bool mWithinRender;
	bool mTextChanged;
	float  mTextStart;                   // position (in pixels) where a code line starts relative to the left of the TextEditor.
	int  mLeftMargin;
	bool mCursorPositionChanged;
	int mColorRangeMin, mColorRangeMax;
	SelectionMode mSelectionMode;

	//Palette mPalette;
	LanguageDefinition mLanguageDefinition;
	RegexList mRegexList;

	bool mCheckMultilineComments;
	ErrorMarkers mErrorMarkers;
	ImVec2 mCharAdvance;
	Coordinates mInteractiveStart, mInteractiveEnd;
};

void vTextEditorInit(TextEditor * m_editor, char *filename);
const char *pestrcasestr(const char *arg1, const char *arg2);


