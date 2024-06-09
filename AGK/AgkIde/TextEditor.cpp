//#### PE: based on , https://github.com/BalazsJako/ImGuiColorTextEdit
//#### PE: 


#include <algorithm>
#include <chrono>
#include <string>
#include <regex>
#include <cmath>
#include "agk.h"

#include "TextEditor.h"
#include "AGKCommands.h"

#ifdef AGK_WINDOWS
#include <direct.h>
#endif

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h" // for imGui::GetCurrentWindow()

#include "files.h"

#define TOGGLEBREAKPOINTX 20 //PE: 03-09-2019 changed from 30 to 26. // 03-03-2020 smaller so larger area to select line text.

static bool TokenizeCStyleNumber(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end);
static bool TokenizeCStyleIdentifier(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end);
static bool TokenizeAGKStyleIdentifier(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end);

int DecodeUTF8Char(const char* str, int *numBytes);
std::string UnicodeToUTF8(unsigned int codepoint);

TextEditor::Palette mPalette; //Moved out of class so all editors use the same setup.

//const TextEditor::Palette& GetPalette() const { return mPalette; }


char WordOnlyAllowBySide[] = " \t()[]!@#$%^&*+/\\:;|`-\",.'\0";
extern KeywordHelp *sKeywordHelp[256], *sKeyNext;
extern int keyboard_layout;
extern std::map<std::string, TextEditor::SymbolsValues> symbolsCollectList[2];

struct BookmarkValues
{
	int lineno;
	TextEditor * m_InsideEditor;
};
extern std::unordered_map<std::int32_t, BookmarkValues> AllBookmark[2];



float roundByTabSize(float x,int tabsize,float spacesize,float startx)
{
	//float oldx = x;
	int rounding = (int)(x / float(tabsize*spacesize));
	rounding *= (tabsize*spacesize);
	if (rounding < startx)
		rounding = startx;
	return(rounding);
}

template<class InputIt1, class InputIt2, class BinaryPredicate>
bool equals(InputIt1 first1, InputIt1 last1,
	InputIt2 first2, InputIt2 last2, BinaryPredicate p)
{
	for (; first1 != last1 && first2 != last2; ++first1, ++first2) 
	{
		if (!p(*first1, *first2))
			return false;
	}
	return first1 == last1 && first2 == last2;
}

const char *pestrcasestr(const char *arg1, const char *arg2)
{
	if (!arg1)
		return NULL;
	if (!arg2)
		return NULL;
	if (strlen(arg2) > strlen(arg1))
		return NULL;

	const char *a, *b;
	for (;*arg1;*arg1++) {

		a = arg1;
		b = arg2;

		//PE: \t | 32 == ) | 32
//		while ((*a++ | 32) == (*b++ | 32))
		while (toupper(*a++) == toupper(*b++))
			if (!*b)
				return (arg1);

	}

	return(NULL);
}
int pecasepos(const char *arg1, const char *arg2)
{
	const char *a, *b, *start;
	start = arg1;
	for (;*arg1;*arg1++) {

		a = arg1;
		b = arg2;

		while ((*a++ | 32) == (*b++ | 32))
			if (!*b)
				return (arg1-start);

	}

	return(-1);
}

void UTF8ToUniCodePoint(char *source)
{
	if (source == NULL)
		return;
	unsigned char *s, *d;
	s = (unsigned char *)source;
	d = (unsigned char *)source;
	while (*s != 0) {

		if (s[1] != 0 && s[0] == 0xc2) {
			s++;
		}
		else if (s[1] != 0 && s[0] == 0xc3) {
			s[1] += 0x40;
			s++;
		}
		else if (s[1] == 0xA0 && s[0] == 0xc2) { //convert non break spaces to space.
			s[1] = ' ';
			s++;
		}
		else if (s[0] == 0xa0) { // convert non break spaces to space.
			s[0] = ' ';
		}

		*d++ = *s++;
	}
	*d = 0;
}

TextEditor::TextEditor()
	: mLineSpacing(1.0f)
	, mUndoIndex(0)
	, mTabSize(4) 
	, mOverwrite(false)
	, mReadOnly(false)
	, mWithinRender(false)
	, mScrollToCursor(false)
	, mTextChanged(false)
	, mTextStart(20.0f)
	, mLeftMargin(20) //PE: (16) Increased a bit to allow more easy way to toggle breakpoints.
	, mColorRangeMin(0)
	, mColorRangeMax(0)
	, mSelectionMode(SelectionMode::Normal)
	, mCheckMultilineComments(true)
{
	if(pref.editorPalette == 4)
		SetPalette(GetCustomPalette()); //Default Dark Style.
	else if (pref.editorPalette == 3)
		SetPalette(GetRetroBluePalette()); //Default Dark Style.
	else if (pref.editorPalette == 2)
		SetPalette(GetLightPalette()); //Default Dark Style.
	else if (pref.editorPalette == 1)
		SetPalette(GetDarkPalette()); //Default Dark Style.
	else
		SetPalette(GetAGKPalette()); //Default Dark Style.
	SetLanguageDefinition(LanguageDefinition::AGK()); //Default AGK.
	mLines.push_back(Line());
}


TextEditor::~TextEditor()
{
}

void TextEditor::SetLanguageDefinition(const LanguageDefinition & aLanguageDef)
{
	mLanguageDefinition = aLanguageDef;
	mRegexList.clear();

	for (auto& r : mLanguageDefinition.mTokenRegexStrings)
		mRegexList.push_back(std::make_pair(std::regex(r.first, std::regex_constants::optimize), r.second));
}

void TextEditor::SetPalette(const Palette & aValue)
{
	mPalette = aValue;
}

int TextEditor::AppendBuffer(std::string& aBuffer, char chr, int aIndex)
{
	if (chr != '\t')
	{
		aBuffer.push_back(chr);
		return aIndex + 1;
	}
	else
	{
		return aIndex;
	}
}

std::string TextEditor::GetText(const Coordinates & aStart, const Coordinates & aEnd) const
{
	std::string result;

	int prevLineNo = aStart.mLine;
	for (auto it = aStart; it <= aEnd; Advance(it))
	{
		if (prevLineNo != it.mLine && it.mLine < (int) mLines.size())
			result.push_back('\n');

		if (it == aEnd) {
			break;
		}

		prevLineNo = it.mLine;
		const auto& line = mLines[it.mLine];
		if (!line.empty() && it.mColumn < (int)line.size()) {
			//UTF8: Convert codepoint to utf8 here.
			if (line[it.mColumn].mInt > 0) {

				std::string ms = UnicodeToUTF8(line[it.mColumn].mInt);
				for (int a = 0; a < ms.size(); a++) {
					result.push_back(ms[a]);
				}
			}
			else {
				result.push_back(line[it.mColumn].mChar);
			}
		}
	}

	return result;
}

TextEditor::Coordinates TextEditor::GetActualCursorCoordinates() const
{
	return SanitizeCoordinates(mState.mCursorPosition);
}

TextEditor::Coordinates TextEditor::SanitizeCoordinates(const Coordinates & aValue) const
{
	auto line = aValue.mLine;
	auto column = aValue.mColumn;

	if (line >= (int)mLines.size())
	{
		if (mLines.empty())
		{
			line = 0;
			column = 0;
		}
		else
		{
			line = (int)mLines.size() - 1;
			column = (int)mLines[line].size();
		}
	}
	else
	{
		column = mLines.empty() ? 0 : std::min((int)mLines[line].size(), aValue.mColumn);
	}

	return Coordinates(line, column);
}

void TextEditor::Advance(Coordinates & aCoordinates) const
{
	if (aCoordinates.mLine < (int)mLines.size())
	{
		auto& line = mLines[aCoordinates.mLine];

		if (aCoordinates.mColumn + 1 < (int)line.size())
			++aCoordinates.mColumn;
		else
		{
			++aCoordinates.mLine;
			aCoordinates.mColumn = 0;
		}
	}
}

void TextEditor::DeleteRange(const Coordinates & aStart, const Coordinates & aEnd)
{
	assert(aEnd >= aStart);
	assert(!mReadOnly);

	if (aEnd == aStart)
		return;

	if (aStart.mLine == aEnd.mLine)
	{
		auto& line = mLines[aStart.mLine];
		if (aStart.mColumn < (int)line.size()) {
			if (aEnd.mColumn >= (int)line.size())
				line.erase(line.begin() + aStart.mColumn, line.end());
			else
				line.erase(line.begin() + aStart.mColumn, line.begin() + aEnd.mColumn);
		}
	}
	else
	{
		auto& firstLine = mLines[aStart.mLine];
		auto& lastLine = mLines[aEnd.mLine];
		//PE: Bug firstline.end = 0 , aStart.mColumn = 2 ? (line empty)
		firstLine.erase(firstLine.begin() + aStart.mColumn, firstLine.end());
		lastLine.erase(lastLine.begin(), lastLine.begin() + aEnd.mColumn);

		if (aStart.mLine < aEnd.mLine)
			firstLine.insert(firstLine.end(), lastLine.begin(), lastLine.end());

		if (aStart.mLine < aEnd.mLine)
			RemoveLine(aStart.mLine + 1, aEnd.mLine + 1);
	}

	mTextChanged = true;
}

int TextEditor::InsertTextAt(Coordinates& /* inout */ aWhere, const char * aValue)
{
	assert(!mReadOnly);

	int totalLines = 0;
	auto chr = *aValue;
	while (chr != '\0')
	{
		assert(!mLines.empty());

		if (chr == '\r')
		{
			// skip
		}
		else if (chr == '\n')
		{
			if (aWhere.mColumn < (int)mLines[aWhere.mLine].size())
			{
				auto& newLine = InsertLine(aWhere.mLine + 1);
				auto& line = mLines[aWhere.mLine];
				newLine.insert(newLine.begin(), line.begin() + aWhere.mColumn, line.end());
				line.erase(line.begin() + aWhere.mColumn, line.end());
			}
			else
			{
				InsertLine(aWhere.mLine + 1);
			}
			++aWhere.mLine;
			aWhere.mColumn = 0;
			++totalLines;
		}
		else
		{
			auto& line = mLines[aWhere.mLine];
			//UTF8: Need UTF8 to codepoint here.
			char *utfconvert = (char *) aValue;
			unsigned char chr = *utfconvert;

			int numbytes = 0;
			int utfchar = -1;
			if (utfconvert) {

				if( *(utfconvert+1) != '\0' ) {

					utfchar = DecodeUTF8Char(utfconvert, &numbytes);
				}
			}

			if (utfchar != -1 && numbytes > 1) {
				bool mcom = false;
				if (aWhere.mColumn <= line.size())
					line.insert(line.begin() + aWhere.mColumn, Glyph(chr, PaletteIndex::Default, mcom, utfchar) );
				aValue += numbytes - 1;
			}
			else {
				if(aWhere.mColumn <= line.size() )
					line.insert(line.begin() + aWhere.mColumn, Glyph(chr, PaletteIndex::Default));
			}
			++aWhere.mColumn;
		}
		chr = *(++aValue);

		mTextChanged = true;
	}

	return totalLines;
}

void TextEditor::AddUndo(UndoRecord& aValue)
{
	assert(!mReadOnly);

	mUndoBuffer.resize(mUndoIndex + 1);
	mUndoBuffer.back() = aValue;
	++mUndoIndex;
}

TextEditor::Coordinates TextEditor::ScreenPosToCoordinates(const ImVec2& aPosition, bool precision) const
{
	//PE: We have roundins when using different colors, use precision=true is more precise.

	ImVec2 origin = ImGui::GetCursorScreenPos();
	ImVec2 local(aPosition.x - origin.x, aPosition.y - origin.y);

	//PE: code folding , we need to exclude all non visible lines before finding the lineNo.
	//PE: We need to start in the top line, and count until we reach (int)floor(local.y / mCharAdvance.y)
	//PE: ignoring non visible lines.
	//PE: for speed this has now been prerecorded in m_iNewLineNo.

	int lineNo;
	if (pref.iEnableCodeFolding && ((int)floor(local.y / mCharAdvance.y)) < MAXPRECHECKLINES ) {

		lineNo = m_iNewLineNo[(int)floor(local.y / mCharAdvance.y)];

		if (lineNo < 0 || lineNo >(int)mLines.size())
			lineNo = std::max(0, (int)floor(local.y / mCharAdvance.y));

		//Not visible find next.
		if (!m_bLineVisible[lineNo]) {
			while (!m_bLineVisible[lineNo] && lineNo < mLines.size() && lineNo < MAXPRECHECKLINES)
				lineNo++;
		}

		if(lineNo < 0 || lineNo >(int)mLines.size())
			lineNo = std::max(0, (int)floor(local.y / mCharAdvance.y));
	}
	else
		lineNo = std::max(0, (int)floor(local.y / mCharAdvance.y));

	
	/*
		Compute columnCoord according to text size
	*/

	int 		columnCoord 			= 0;
	float       columnWidth             = 0.0f;
	std::string cumulatedString			= "";
	float 		cumulatedStringWidth[2] = {0.0f, 0.0f}; //( [0] is the lastest, [1] is the previous. I use that trick to check where cursor is exactly (important for tabs)

	if (lineNo >= 0 && lineNo < (int)mLines.size())
	{
		auto&       line = mLines.at(lineNo);

		// First we find the hovered column coord.
		while (mTextStart + cumulatedStringWidth[0] < local.x &&
			(size_t)columnCoord < line.size())
		{
			if (!precision)
			{
				cumulatedStringWidth[1] = cumulatedStringWidth[0];
				cumulatedString += line[columnCoord].mChar;

				cumulatedStringWidth[0] = ImGui::CalcTextSize(cumulatedString.c_str()).x;

				columnWidth = (cumulatedStringWidth[0] - cumulatedStringWidth[1]);
			}
			else
//				cumulatedStringWidth[0] = TextDistanceToLineStart(Coordinates(lineNo, columnCoord + 1));
				cumulatedStringWidth[0] = TextDistanceToLineStart(Coordinates(lineNo, columnCoord + 1))+3.0; //JUSTIFY MOUSE
			columnCoord++;
		}

		if (!precision)
		{
			// Then we reduce by 1 column coord if cursor is on the left side of the hovered column.
			if (mTextStart + cumulatedStringWidth[0] - columnWidth / 2.0f > local.x)
					columnCoord = std::max(0, columnCoord - 1);
		}
	}

	return SanitizeCoordinates(Coordinates(lineNo, columnCoord));
}


// Seletec test: Rem test3#$ing s444s 222s2 ###AAA@@@ A# @aa tmp()
#define ISALETTERDIGIT "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"

TextEditor::Coordinates TextEditor::FindWordStart(const Coordinates & aFrom) const
{
	Coordinates at = aFrom;
	if (at.mLine >= (int)mLines.size())
		return at;

	auto& line = mLines[at.mLine];

	if (at.mColumn >= (int)line.size())
		return at;

	auto cstart = (PaletteIndex)line[at.mColumn].mColorIndex;
	if (cstart == PaletteIndex::Comment || cstart == PaletteIndex::String) {
		if (strchr(ISALETTERDIGIT, line[at.mColumn].mChar)) {
			while (at.mColumn > 0)
			{
				if(!strchr(ISALETTERDIGIT, line[at.mColumn - 1].mChar))
					break;
				if (line[at.mColumn - 1].mChar == ' ' || line[at.mColumn - 1].mChar == '\t')
					break;
				--at.mColumn;
			}

		}
		else {
			while (at.mColumn > 0)
			{
				if (strchr(ISALETTERDIGIT, line[at.mColumn - 1].mChar))
					break;
				if (line[at.mColumn - 1].mChar == ' ' || line[at.mColumn - 1].mChar == '\t')
					break;
				--at.mColumn;
			}
		}

	}
	else {
		while (at.mColumn > 0)
		{
			if (cstart != (PaletteIndex)line[at.mColumn - 1].mColorIndex)
				break;
			--at.mColumn;
		}
	}
	return at;
}

TextEditor::Coordinates TextEditor::FindWordEnd(const Coordinates & aFrom) const
{
	Coordinates at = aFrom;
	if (at.mLine >= (int)mLines.size())
		return at;

	auto& line = mLines[at.mLine];

	if (at.mColumn >= (int)line.size())
		return at;

	auto cstart = (PaletteIndex)line[at.mColumn].mColorIndex;
	if (cstart == PaletteIndex::Comment || cstart == PaletteIndex::String ) {
		if (strchr(ISALETTERDIGIT, line[at.mColumn].mChar)) {
			while (at.mColumn < (int)line.size())
			{
				if (!strchr(ISALETTERDIGIT, line[at.mColumn].mChar))
					break;
				if (line[at.mColumn].mChar == ' ' || line[at.mColumn].mChar == '\t')
					break;
				++at.mColumn;
			}
		}
		else
		{
			while (at.mColumn < (int)line.size())
			{
				if (strchr(ISALETTERDIGIT, line[at.mColumn].mChar))
					break;
				if (line[at.mColumn].mChar == ' ' || line[at.mColumn].mChar == '\t')
					break;
				++at.mColumn;
			}
		}
	}
	else {
		while (at.mColumn < (int)line.size())
		{
			if (cstart != (PaletteIndex)line[at.mColumn].mColorIndex) {
				break;
			}
			++at.mColumn;
		}
	}
	return at;
}

bool TextEditor::IsOnWordBoundary(const Coordinates & aAt) const
{
	if (aAt.mLine >= (int)mLines.size() || aAt.mColumn == 0)
		return true;

	auto& line = mLines[aAt.mLine];
	if (aAt.mColumn >= (int)line.size())
		return true;

	//return line[aAt.mColumn].mColorIndex != line[aAt.mColumn - 1].mColorIndex;
	//Allow single char selections.
	return false;
}

void TextEditor::RemoveLine(int aStart, int aEnd)
{
	assert(!mReadOnly);
	assert(aEnd >= aStart);
	assert(mLines.size() > (size_t)(aEnd - aStart));
	int delLines = aEnd - aStart;
	if (delLines <= 0 || delLines >= mLines.size() - 1)
		delLines = 1;

	//PE: code folding.
	if (pref.iEnableCodeFolding && aStart < MAXPRECHECKLINES) {
		//m_bLineVisible[aStart] = true;
		int movelinestep = delLines - 1;
		for (int i = aStart; i+ movelinestep < mLines.size() - 1; i++) {
			if (i + movelinestep < MAXPRECHECKLINES) {
				m_bLineVisible[i - 1] = m_bLineVisible[i + movelinestep];
				m_iNewLineNo[i - 1] = m_iNewLineNo[i + movelinestep];
				m_iHiddenLineNo[i - 1] = m_iHiddenLineNo[i + movelinestep];
				m_bFoldingActive[i - 1] = m_bFoldingActive[i + movelinestep];
				m_bFoldingPossible[i - 1] = m_bFoldingPossible[i + movelinestep];
				m_bFoldingStartWord[i - 1] = m_bFoldingStartWord[i + movelinestep];
				m_bFoldingEndWord[i - 1] = m_bFoldingEndWord[i + movelinestep];
				m_bCheckLine[i - 1] = m_bCheckLine[i + movelinestep];
				m_bFirstCheck[i - 1] = m_bFirstCheck[i + movelinestep];
			}
		}
	}


	ErrorMarkers etmp;
	for (auto& i : mErrorMarkers)
	{
		ErrorMarkers::value_type e(i.first >= aStart ? i.first - 1 : i.first, i.second);
		if (e.first >= aStart && e.first <= aEnd)
			continue;
		etmp.insert(e);
	}
	mErrorMarkers = std::move(etmp);

	Breakpoints btmp;
	for (auto i : mBreakpoints)
	{
		if (i >= aStart && i <= aEnd)
			continue;
		btmp.insert(i >= aStart ? i - delLines : i);
	}
	mBreakpoints = std::move(btmp);

	Breakpoints btmp3;
	for (auto i : mFoldingActive)
	{
		if (i >= aStart && i <= aEnd)
			continue;
		btmp3.insert(i >= aStart ? i - delLines : i);
	}
	mFoldingActive = std::move(btmp3);
	


	Breakpoints btmp2;
	for (auto i : bpts)
	{
		if (i >= aStart && i <= aEnd)
			continue;
		btmp2.insert(i >= aStart ? i - delLines : i);
	}
	bpts = std::move(btmp2);
	


	Bookmarks mtmp;
	for (auto i : mBookMarks)
	{
		if (i >= aStart && i <= aEnd)
			continue;
		mtmp.insert(i >= aStart ? i - delLines : i);
	}
	mBookMarks = std::move(mtmp);

	mLines.erase(mLines.begin() + aStart, mLines.begin() + aEnd);

	Colorize(aStart-1, 6); //PE:

	assert(!mLines.empty());

	mTextChanged = true;
}

void TextEditor::RemoveLine(int aIndex)
{
	assert(!mReadOnly);
	assert(mLines.size() > 1);

	//PE: code folding.
	if (pref.iEnableCodeFolding && aIndex < MAXPRECHECKLINES) {
		//m_bLineVisible[aIndex] = true;
		for (int i = aIndex ; i < mLines.size() - 1; i++) {
			if (i < MAXPRECHECKLINES) {
				m_bLineVisible[i - 1] = m_bLineVisible[i];
				m_iNewLineNo[i - 1] = m_iNewLineNo[i];
				m_iHiddenLineNo[i - 1] = m_iHiddenLineNo[i];
				m_bFoldingActive[i - 1] = m_bFoldingActive[i];
				m_bFoldingPossible[i - 1] = m_bFoldingPossible[i];
				m_bFoldingStartWord[i - 1] = m_bFoldingStartWord[i];
				m_bFoldingEndWord[i - 1] = m_bFoldingEndWord[i];
				m_bCheckLine[i - 1] = m_bCheckLine[i];
				m_bFirstCheck[i - 1] = m_bFirstCheck[i];
			}
		}
	}

	ErrorMarkers etmp;
	for (auto& i : mErrorMarkers)
	{
		ErrorMarkers::value_type e(i.first > aIndex ? i.first - 1 : i.first, i.second);
		if (e.first - 1 == aIndex)
			continue;
		etmp.insert(e);
	}
	mErrorMarkers = std::move(etmp);

	Breakpoints btmp;
	for (auto i : mBreakpoints)
	{
		if (i == aIndex)
			continue;
		btmp.insert(i >= aIndex ? i - 1 : i);
	}
	mBreakpoints = std::move(btmp);

	Breakpoints btmp3;
	for (auto i : mFoldingActive)
	{
		if (i == aIndex)
			continue;
		btmp3.insert(i >= aIndex ? i - 1 : i);
	}
	mFoldingActive = std::move(btmp3);


	Breakpoints btmp2;
	for (auto i : bpts)
	{
		if (i == aIndex)
			continue;
		btmp2.insert(i >= aIndex ? i - 1 : i);
	}
	bpts = std::move(btmp2);

	
	Bookmarks mtmp;
	for (auto i : mBookMarks)
	{
		if (i == aIndex)
			continue;
		mtmp.insert(i >= aIndex ? i - 1 : i);
	}
	mBookMarks = std::move(mtmp);

		mLines.erase(mLines.begin() + aIndex);
	assert(!mLines.empty());

	mTextChanged = true;
}

TextEditor::Line& TextEditor::InsertLine(int aIndex)
{
	assert(!mReadOnly);

	//PE: code folding.
	if (pref.iEnableCodeFolding && aIndex < MAXPRECHECKLINES) {
		m_bLineVisible[aIndex] = true;
		for (int i = mLines.size()-1; i > aIndex; i--) {
			if (i + 1 < MAXPRECHECKLINES) {
				m_bLineVisible[i + 1] = m_bLineVisible[i];
				m_iNewLineNo[i + 1] = m_iNewLineNo[i];
				m_iHiddenLineNo[i + 1] = m_iHiddenLineNo[i];
				m_bFoldingActive[i + 1] = m_bFoldingActive[i];
				m_bFoldingPossible[i + 1] = m_bFoldingPossible[i];
				m_bFoldingStartWord[i + 1] = m_bFoldingStartWord[i];
				m_bFoldingEndWord[i + 1] = m_bFoldingEndWord[i];
				m_bCheckLine[i + 1] = m_bCheckLine[i];
				m_bFirstCheck[i + 1] = m_bFirstCheck[i];
			}
		}


	}

	auto& result = *mLines.insert(mLines.begin() + aIndex, Line());

	ErrorMarkers etmp;
	for (auto& i : mErrorMarkers)
		etmp.insert(ErrorMarkers::value_type(i.first >= aIndex ? i.first + 1 : i.first, i.second));
	mErrorMarkers = std::move(etmp);

	Breakpoints btmp;
	for (auto i : mBreakpoints)
		btmp.insert(i >= aIndex ? i + 1 : i);
	mBreakpoints = std::move(btmp);

	Breakpoints btmp3;
	for (auto i : mFoldingActive)
		btmp3.insert(i >= aIndex ? i + 1 : i);
	mFoldingActive = std::move(btmp3);

	Breakpoints btmp2;
	for (auto i : bpts)
		btmp2.insert(i >= aIndex ? i + 1 : i);
	bpts = std::move(btmp2);

	Bookmarks mtmp;
	for (auto i : mBookMarks)
		mtmp.insert(i >= aIndex ? i + 1 : i);
	mBookMarks = std::move(mtmp);

	
	return result;
}

std::string TextEditor::GetWordUnderCursor() const
{
	auto c = GetCursorPosition();
	return GetWordAt(c);
}

std::string TextEditor::GetWordAt(const Coordinates & aCoords,bool variable,bool includefunctions) const
{
	auto start = FindWordStart(aCoords);
	auto end = FindWordEnd(aCoords);

	if (variable) {
		if (start != end) {
			auto& line = mLines[end.mLine];
			if (aCoords.mColumn > 0) {
				if (line[aCoords.mColumn].mChar == '#') {
					start = FindWordStart(Coordinates(aCoords.mLine, aCoords.mColumn - 1));
					end = FindWordEnd(Coordinates(aCoords.mLine, aCoords.mColumn - 1));
				}
				else if (line[aCoords.mColumn].mChar == '$') {
					start = FindWordStart(Coordinates(aCoords.mLine, aCoords.mColumn - 1));
					end = FindWordEnd(Coordinates(aCoords.mLine, aCoords.mColumn - 1));
				}
				else if (line[aCoords.mColumn].mChar == '[') {
					start = FindWordStart(Coordinates(aCoords.mLine, aCoords.mColumn - 1));
					end = FindWordEnd(Coordinates(aCoords.mLine, aCoords.mColumn - 1));
				}
			}
			if (end.mColumn < (int)line.size()) {
				if (line[end.mColumn].mChar == '#') {
					end.mColumn++;
				}
				else if (line[end.mColumn].mChar == '$') {
					end.mColumn++;
				}
				else if (line[end.mColumn].mChar == '(') {
					if(!includefunctions)
						start = end; // disable function.
				}
				
				if (end.mColumn < (int)line.size()) {
					if (line[end.mColumn].mChar == '[' || line[aCoords.mColumn].mChar == '[') {
						//Add until ]
						while (end.mColumn < (int)line.size()) {
							if (line[end.mColumn].mChar == ']') {
								end.mColumn++;
								break;
							}
							end.mColumn++;
						}
					}
				}
			}

			// . variables.
			if (end.mColumn < (int)(line.size()-1)) {
				if (line[end.mColumn].mChar == '.') {
					end.mColumn++;
					//find new word from here:
					if (end.mColumn < (int)(line.size()-1))
						end = FindWordEnd(Coordinates(end.mLine, end.mColumn ));

					if (end.mColumn < (int)(line.size())) {
						//recheck []
						if (line[end.mColumn].mChar == '#') {
							end.mColumn++;
						}
						else if (line[end.mColumn].mChar == '$') {
							end.mColumn++;
						}
						else if (line[end.mColumn].mChar == '(') {
							if (!includefunctions)
								start = end; // disable function.
						}
						else if (line[end.mColumn].mChar == '[' || line[aCoords.mColumn].mChar == '[') {
							//Add until ]
							while (end.mColumn < (int)line.size()) {
								if (line[end.mColumn].mChar == ']') {
									end.mColumn++;
									break;
								}
								end.mColumn++;
							}
						}
					}

				}

			}


		}
	}


	std::string r;

	for (auto it = start; it < end; Advance(it))
		r.push_back(mLines[it.mLine][it.mColumn].mChar);

	return r;
}


static char cHelp[MAX_PATH];

void TextEditor::Help( void )
{
	strcpy(cHelp, "");

	//Try to locate what we are searching for.
	if (HasSelection())
	{
		if (strlen(GetSelectedText().c_str()) < MAX_PATH) {
			strcpy(cHelp, GetSelectedText().c_str());
		}

	}
	else {

		std::string wuc = GetWordUnderCursor();
		if (strlen(wuc.c_str()) < MAX_PATH && strlen(wuc.c_str()) > 1) {
			strcpy(cHelp, wuc.c_str());
		}
	}

	if (strlen(cHelp) < 2)
		return;

	//Try to find help.
	char curDir[MAX_PATH];

#ifdef AGK_WINDOWS
	_getcwd(&curDir[0], MAX_PATH);
#else
	getcwd(&curDir[0], MAX_PATH);
#endif

	int index = tolower( char(cHelp[0]) );
	uString usHelp = cHelp;
	usHelp.Lower();
	sKeyNext = sKeywordHelp[index];
	if (sKeyNext && sKeyNext->m_pNext) {
		sKeyNext = sKeyNext->m_pNext;

		while (sKeyNext != NULL) {
			if (strcmp(usHelp.GetStr(), sKeyNext->m_cLowerCommand) == 0) {
				//found it display page.
				if (sKeyNext->m_cCommandPath.GetLength() > 0 ) {
					
					//built in help
					if (pref.bBrowserHelp == false) {
						processhelp((char*)sKeyNext->m_cCommandPath.GetStr(), true);
						ImGui::SetWindowFocus(ICON_MD_HELP  " Help");
					}
					//browser help
					else {
						strcat(curDir, "\\");
						strcat(curDir, (char*)sKeyNext->m_cCommandPath.GetStr());

						agk::OpenBrowser(curDir);
					}
					
					break;
				}
			}
			sKeyNext = sKeyNext->m_pNext;
		}

	}

	return;
}


void TextEditor::BrowserHelp(void)
{
	strcpy(cHelp, "");

	//Try to locate what we are searching for.
	if (HasSelection())
	{
		if (strlen(GetSelectedText().c_str()) < MAX_PATH) {
			strcpy(cHelp, GetSelectedText().c_str());
		}

	}
	else {

		std::string wuc = GetWordUnderCursor();
		if (strlen(wuc.c_str()) < MAX_PATH && strlen(wuc.c_str()) > 1) {
			strcpy(cHelp, wuc.c_str());
		}
	}

	if (strlen(cHelp) < 2)
		return;

	//Try to find help.
	char curDir[MAX_PATH];

#ifdef AGK_WINDOWS
	_getcwd(&curDir[0], MAX_PATH);
#else
	getcwd(&curDir[0], MAX_PATH);
#endif

	int index = tolower(char(cHelp[0]));
	uString usHelp = cHelp;
	usHelp.Lower();
	sKeyNext = sKeywordHelp[index];
	if (sKeyNext && sKeyNext->m_pNext) {
		sKeyNext = sKeyNext->m_pNext;

		while (sKeyNext != NULL) {
			if (strcmp(usHelp.GetStr(), sKeyNext->m_cLowerCommand) == 0) {
				//found it display page.
				if (sKeyNext->m_cCommandPath.GetLength() > 0) {

					//browser help
					strcat(curDir, "\\");
					strcat(curDir, (char*)sKeyNext->m_cCommandPath.GetStr());

					agk::OpenBrowser(curDir);

				
					break;
				}
			}
			sKeyNext = sKeyNext->m_pNext;
		}

	}

	return;
}


static float oldscrollX;
static float oldscrollY;
static uString search_items[11];
static int combo_search_entrys = 0;
static char ReplaceMsg[MAX_PATH];
static char lastKeywordSelected[MAX_PATH];
static bool bMatchCase = false;
static bool bMatchWord = false;
static bool replaceactive = true;
static bool bSetInputFocus = false;
static float lastKeySearch = -1.0f;
static char cSearch[MAX_PATH] = "\0";
static char cSearchReplace[MAX_PATH] = "\0";
static bool bFindPrevStartOfLine = false;
extern cFolderItem::sFolderFiles *pDragDropFile;
extern char *pDragDropText;

std::string correct_non_utf_8(std::string *str)
{
	int i, f_size = str->size();
	unsigned char c, c2, c3, c4;
	std::string to;
	to.reserve(f_size);

	for (i = 0; i<f_size; i++) {
		c = (unsigned char)(*str)[i];
		if (c<32) {//control char
			if (c == 9 || c == 10 || c == 13) {//allow only \t \n \r
				to.append(1, c);
			}
			continue;
		}
		else if (c<127) {//normal ASCII
			to.append(1, c);
			continue;
		}
		else if (c<160) {//control char (nothing should be defined here either ASCI, ISO_8859-1 or UTF8, so skipping)
			if (c2 == 128) {//fix microsoft mess, add euro
				to.append(1, 226);
				to.append(1, 130);
				to.append(1, 172);
			}
			if (c2 == 133) {//fix IBM mess, add NEL = \n\r
				to.append(1, 10);
				to.append(1, 13);
			}
			continue;
		}
		else if (c<192) {//invalid for UTF8, converting ASCII
			to.append(1, (unsigned char)194);
			to.append(1, c);
			continue;
		}
		else if (c<194) {//invalid for UTF8, converting ASCII
			to.append(1, (unsigned char)195);
			to.append(1, c - 64);
			continue;
		}
		else if (c<224 && i + 1<f_size) {//possibly 2byte UTF8
			c2 = (unsigned char)(*str)[i + 1];
			if (c2>127 && c2<192) {//valid 2byte UTF8
				if (c == 194 && c2<160) {//control char, skipping
					;
				}
				else {
					to.append(1, c);
					to.append(1, c2);
				}
				i++;
				continue;
			}
		}
		else if (c<240 && i + 2<f_size) {//possibly 3byte UTF8
			c2 = (unsigned char)(*str)[i + 1];
			c3 = (unsigned char)(*str)[i + 2];
			if (c2>127 && c2<192 && c3>127 && c3<192) {//valid 3byte UTF8
				to.append(1, c);
				to.append(1, c2);
				to.append(1, c3);
				i += 2;
				continue;
			}
		}
		else if (c<245 && i + 3<f_size) {//possibly 4byte UTF8
			c2 = (unsigned char)(*str)[i + 1];
			c3 = (unsigned char)(*str)[i + 2];
			c4 = (unsigned char)(*str)[i + 3];
			if (c2>127 && c2<192 && c3>127 && c3<192 && c4>127 && c4<192) {//valid 4byte UTF8
				to.append(1, c);
				to.append(1, c2);
				to.append(1, c3);
				to.append(1, c4);
				i += 3;
				continue;
			}
		}
		//invalid UTF8, converting ASCII (c>245 || string too short for multi-byte))
		to.append(1, (unsigned char)195);
		to.append(1, c - 64);
	}
	return to;
}

void TextEditor::ToggleSeachReplace(bool change, bool forceadd)
{
	if (change && bSearchActive || forceadd ) {
		if (strlen(cSearch) > 0 && strlen(cSearch) < MAX_PATH ) {
			int a,afound=false;
			for ( a = 9; a >= 0; a--) {
				if (search_items[a].GetStr()) {
					if (strcmp(search_items[a].GetStr(), cSearch) == 0) {
						afound = true;
						break;
					}
				}
				else {
					search_items[a] = "";
				}
			}
			if( !afound ) {
				for (a = 9; a > 0; a--) {
					search_items[a] = search_items[a - 1];
				}
				search_items[0] = cSearch;
				combo_search_entrys++;
			}
			if (combo_search_entrys >= 9) combo_search_entrys = 9;
		}
	}
	else {
		if (HasSelection())
		{
			if (strlen(GetSelectedText().c_str()) < MAX_PATH) {
				if (mState.mSelectionStart.mLine == mState.mSelectionEnd.mLine) {
					std::string ts = GetSelectedText().c_str();
					strcpy(cSearch, correct_non_utf_8( &ts ).c_str() );
				}
			}

		} else {

			std::string wuc = GetWordUnderCursor();
			if( strlen(wuc.c_str()) < MAX_PATH && strlen(wuc.c_str()) > 1 ) {
				std::string ts = wuc.c_str();
				strcpy(cSearch, correct_non_utf_8(&ts).c_str() );
			}

		}

		bSetInputFocus = true;
	}
	
	if (change) {
		bSearchActive = 1 - bSearchActive;
	}
	return;
}

static inline int CharToUtf8(char* buf, int buf_size, unsigned int c)
{
	if (c < 0x80)
	{
		buf[0] = (char)c;
		return 1;
	}
	if (c < 0x800)
	{
		if (buf_size < 2) return 0;
		buf[0] = (char)(0xc0 + (c >> 6));
		buf[1] = (char)(0x80 + (c & 0x3f));
		return 2;
	}
	if (c >= 0xdc00 && c < 0xe000)
	{
		return 0;
	}
	if (c >= 0xd800 && c < 0xdc00)
	{
		if (buf_size < 4) return 0;
		buf[0] = (char)(0xf0 + (c >> 18));
		buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
		buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[3] = (char)(0x80 + ((c) & 0x3f));
		return 4;
	}
	//else if (c < 0x10000)
	{
		if (buf_size < 3) return 0;
		buf[0] = (char)(0xe0 + (c >> 12));
		buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[2] = (char)(0x80 + ((c) & 0x3f));
		return 3;
	}
}

static char lastKeyedWord[MAX_PATH+1];
bool bFreezeWord = false;
bool bSuggestActice = false;
bool bEnableSuggest = true;
bool bTabPressed = false;
bool bLookupReadOnly = false;
bool bHideReplaceBut = false;
int  bCursorKeys = 0;
int  iFindDialogAction = 0;
static int freezeSelection = 0;
static std::string freezeWord;
ImVec2 freezePos;
extern char cHoverGetVariable[MAX_PATH];
extern char cHoverVariable[MAX_PATH];
#define MAXVALUESIZE 2048
extern char cHoverValue[MAXVALUESIZE + 1];
extern float fHoverTime;


void TextEditor::Render(const char* aTitle, const ImVec2& aSize, bool aBorder, bool modalopen,char *projectName)
{
	static auto timeStart = std::chrono::system_clock::now();

	mTabSize = pref.mTabSize;

	//return; // fps 1100.
	mWithinRender = true;
	mTextChanged = false;
	mCursorPositionChanged = false;

	bool wtabvisible = ImGui::windowTabVisible();
	bool bEnableFreezeInNextRun = false;

	//ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImGui::ColorConvertU32ToFloat4(mPalette[(int)PaletteIndex::Background]));
//	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::ColorConvertU32ToFloat4(mPalette[(int)PaletteIndex::Background]));

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	//ImGuiDockFamily editorfamily;
	//editorfamily.CompatibleWithFamilyZero = false;
	//editorfamily.ID = 9999;

	ImGui::BeginChild(aTitle, aSize, aBorder, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavInputs);

	ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize(), mPalette[(int)PaletteIndex::Background] );

	/* Compute mCharAdvance regarding to scaled font size (Ctrl + mouse wheel)*/
	const float fontSize = ImGui::CalcTextSize("#").x;
	mCharAdvance = ImVec2(fontSize , ImGui::GetTextLineHeightWithSpacing() * mLineSpacing);	

	ImVec2 winsize = ImGui::GetWindowSize();

	/*
		Keyboard inputs
	*/

	ImGui::PushAllowKeyboardFocus(true);
	ImGuiIO& io = ImGui::GetIO();
	auto shift = io.KeyShift;
	auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
	auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;
	if (keyboard_layout == 0) {
		ctrl = io.KeyCtrl;
		alt = io.KeyAlt;
	}

	bool is_osx = io.ConfigMacOSXBehaviors;
	bTabPressed = false;
	bCursorKeys = 0;

	if (ImGui::IsWindowFocused() && wtabvisible)
	{

		ImRect bbwin(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize());
		if (ImGui::IsMouseHoveringRect(bbwin.Min, bbwin.Max))
		{
			if (ImGui::IsWindowHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
		}



		//ImGui::CaptureKeyboardFromApp(true);

		io.WantCaptureKeyboard = true;
		io.WantTextInput = true;
//		if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z))) {
		if (!IsReadOnly() && ctrl==pref.bUndoCtrl && shift==pref.bUndoShift && alt==pref.bUndoAlt && ImGui::IsKeyPressed(pref.iUndoKey)) {
			Undo();
			bFreezeWord = false;
		}
		else if (ctrl == pref.bZoomInCtrl && shift == pref.bZoomInShift && alt == pref.bZoomInAlt && ImGui::IsWindowHovered() && io.MouseWheel != 0.0f) {
			//PE: Mouse Wheel zoom now respect preferences shortcut setting for shift,alt,ctrl.
			if (!pref.iDisableMouseWheelZoom)
			{
				float add = io.MouseWheel;
				if (add > 0) {
					extern int iExternalpressedCTRLkey;
					iExternalpressedCTRLkey = '+';
				}
				else {
					extern int iExternalpressedCTRLkey;
					iExternalpressedCTRLkey = '-';
				}
			}
		}
		else if (!IsReadOnly() && !ctrl && !shift && alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace))) {
			Undo();
			bFreezeWord = false;
		}
		else if (!IsReadOnly() && ctrl==pref.bRedoCtrl && shift==pref.bRedoShift && alt==pref.bRedoAlt && ImGui::IsKeyPressed(pref.iRedoKey)) {
			Redo();
			bFreezeWord = false;
		}
		else if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(0x53)) { // 0x53 = s IsKeyPressed
			saveonreturn = true; // save file on returning.
			//Need to reset keys when we use blocking dialogs , or they can hang.
			io.KeysDown[0x53] = false; // reset key.
			io.KeySuper = false;
			io.KeyCtrl = false;
			io.KeyShift = false;
			agk::KeyUp(0x53); // reset key.
			agk::KeyUp(16); // reset shift key.
			agk::KeyUp(17); // reset key.
			agk::KeyUp(18); // reset key.
			agk::KeyUp(19); // reset key.
			for (int i = 8; i < 273; i++) {
				agk::KeyUp(i);
			}

		}
		else if (!IsReadOnly() && ctrl && shift && !alt && ImGui::IsKeyPressed(0x46)) { // 0x46 = f
			//CTRL+SHIFT+F bring replace dialog up, include advanced search.
			bFreezeWord = false;
			if (ImGui::GetTime() - lastKeySearch >= 0.125) {
				Replace();
				bHideReplaceBut = true;
				lastKeySearch = (float)ImGui::GetTime();
			}

		}
		else if (ctrl==pref.bFindCtrl && shift==pref.bFindShift && alt==pref.bFindAlt && ImGui::IsKeyPressed(pref.iFindKey)) { // 0x46 = f
			bFreezeWord = false;
			replaceactive = false;
			if (ImGui::GetTime() - lastKeySearch >= 0.125) {
				OpenFind();
				lastKeySearch = (float)ImGui::GetTime();
			}
		}
		else if (ctrl == pref.bGoToDefinitionCtrl && shift == pref.bGoToDefinitionShift && alt == pref.bGoToDefinitionAlt && ImGui::IsKeyPressed(pref.iGoToDefinitionKey)) {
			//Go To Definition.
			if (ImGui::GetTime() - lastKeySearch >= 0.125) {

				auto id = GetWordAt(mState.mCursorPosition, true, true);
				if (!id.empty())
				{

					uString szClean = id.c_str();
					szClean.Lower();
					int pos = szClean.FindStr("[");
					if (pos <= 0)
						pos = szClean.FindStr(".");
					if (pos > 0) {
						uString tmp;
						szClean.SubString(tmp, 0, pos);
						szClean = tmp;
					}

					uString szCleanFirst = szClean;
					szCleanFirst.Append("##func");
					auto it2 = symbolsList[activeSymbolslist].find(szCleanFirst.GetStr());
					if (it2 == symbolsList[activeSymbolslist].end())
					{
						it2 = symbolsList[activeSymbolslist].find(szClean.GetStr());
					}
					if (it2 != symbolsList[activeSymbolslist].end()) {
						if (it2->second.m_InsideEditor) {
							int gettotop = (winsize.y / mCharAdvance.y) - 10; //-5 lines from top.
							if (gettotop <= 0)
								gettotop = 15;
							mScrollToCursorAddLines = -gettotop;
							mScrollToCursor = true;

							SetCursorPosition(TextEditor::Coordinates(it2->second.lineno - 1, 0));
							mScrollToCursor = false;
						}
					}
					else {

						auto it = symbolsCollectList[iCollectListActive].find(szClean.GetStr());
						//
						if (it != symbolsCollectList[iCollectListActive].end())
						{
							if (it->second.m_InsideEditor) {

								cProjectItem::sProjectFiles * m_pCurrentFile;
								//m_pCurrentFile = FindFileFromEditor(it->second.m_InsideEditor);
								m_pCurrentFile = FindFileFromEditorMatchSource(it->second.m_InsideEditor);
								if (m_pCurrentFile) {
									cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
									bFocusActiveEditor = true;
								}
								//Focus.

								int gettotop = (winsize.y / mCharAdvance.y) - 10; //-5 lines from top.
								if (gettotop <= 0)
									gettotop = 15;

								if (m_pCurrentFile->m_editor) {
									m_pCurrentFile->m_editor->mScrollToCursorAddLines = -gettotop;
									m_pCurrentFile->m_editor->mScrollToCursor = true;
									m_pCurrentFile->m_editor->SetCursorPosition(TextEditor::Coordinates(it->second.lineno - 1, 0));
									m_pCurrentFile->m_editor->mScrollToCursor = false;
								}
								else {
									it->second.m_InsideEditor->mScrollToCursorAddLines = -gettotop;
									it->second.m_InsideEditor->mScrollToCursor = true;
									it->second.m_InsideEditor->SetCursorPosition(TextEditor::Coordinates(it->second.lineno - 1, 0));
									it->second.m_InsideEditor->mScrollToCursor = false;
								}
							}
						}
					}

				}

				lastKeySearch = (float)ImGui::GetTime();
			}
		}
		// //~
		else if (ctrl == pref.bDuplicateLineCtrl && shift == pref.bDuplicateLineShift && alt == pref.bDuplicateLineAlt && ImGui::IsKeyPressed(pref.iDuplicateLineKey)) {
			
			if (mState.mCursorPosition.mLine+1 < mLines.size()) {

				//Duplicate Line.
				Coordinates oldc = mState.mCursorPosition;
				Coordinates oldss = mState.mSelectionStart;
				Coordinates oldse = mState.mSelectionEnd;

				//PE: New way that dont use the clipboard, some have problems with this ?
				Coordinates newcords = oldc;
				newcords.mColumn = 0;
				Coordinates newcordsend = newcords;
				newcordsend.mLine++;

				std::string linetext = GetText( newcords, newcordsend );
				filechanged = true;
				UndoRecord u;
				u.mBefore = mState;
				u.mAdded = linetext;
				mState.mCursorPosition = newcords;
				u.mAddedStart = GetActualCursorCoordinates();
				mState.mCursorPosition = newcords;
				InsertText(linetext);
				u.mAddedEnd = GetActualCursorCoordinates();
				u.mAfter = mState;
				AddUndo(u);

				/*
				mState.mSelectionStart = mState.mCursorPosition;
				mState.mSelectionStart.mColumn = 0;
				mState.mSelectionEnd = mState.mCursorPosition;
				mState.mSelectionEnd.mColumn = 0;
				mState.mSelectionEnd.mLine++;

				std::string tmpClipText = ImGui::GetClipboardText();

				Copy();
				mState.mCursorPosition = mState.mSelectionEnd;
				mState.mSelectionStart = mState.mSelectionEnd = oldss;
				Paste();

				if (tmpClipText.size() > 0 ) {
					ImGui::SetClipboardText(tmpClipText.c_str() );
				}
				*/
				mState.mCursorPosition = oldc;
				mState.mSelectionStart = oldss;
				mState.mSelectionEnd = oldse;
			}
		}
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(69)) { // 69 = e
			bFreezeWord = false;
			if (ImGui::GetTime() - lastKeySearch >= 0.125) {
				ToggleLineComments();
				lastKeySearch = (float)ImGui::GetTime();
			}
		}
		else if (ctrl && !shift && !alt && (ImGui::IsKeyPressed(190) || ImGui::IsKeyPressed(191)) ) { // 190 = .
			if (ImGui::GetTime() - lastKeySearch >= 0.125) {
				int first = -1,first_mark = 0;
				TextEditor * first_InsideEditor,*best_InsideEditor,*set_InsideEditor;
				int best = -1,best_mark = 0;

				extern int active_bookmark_list;
				extern int current_bookmark;

				set_InsideEditor = 0;

				int list_count = 0;

				current_bookmark++;
				if (current_bookmark >= AllBookmark[active_bookmark_list].size())
					current_bookmark = 0;

				for (std::unordered_map<std::int32_t, BookmarkValues>::iterator it = AllBookmark[active_bookmark_list].begin(); it != AllBookmark[active_bookmark_list].end(); ++it)
				{

					if (first < 0) {
						first = it->second.lineno;
						first_InsideEditor = (TextEditor *) it->second.m_InsideEditor;
						first_mark = list_count;
					}
					if (list_count == current_bookmark) {
						best = it->second.lineno;
						best_InsideEditor = (TextEditor *)it->second.m_InsideEditor;
						best_mark = list_count;
					}

					list_count++;
				}

				int set_line = 0;
				if (best >= 0) {
//					mCurrentMark = best_mark;
					set_InsideEditor = best_InsideEditor;
					set_line = best;
				}
				else {
					if (first >= 0) {
//						mCurrentMark = first_mark;
//						mCurrentMark = 1;
						set_InsideEditor = first_InsideEditor;
						set_line = first;
					}
				}
				if (set_InsideEditor) {
					cProjectItem::sProjectFiles * m_pCurrentFile;
					m_pCurrentFile = FindFileFromEditorMatchSource(set_InsideEditor);
					if (m_pCurrentFile) {

						cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
						bFocusActiveEditor = true;
						if(set_line > 0)
							m_pCurrentFile->m_editor->mState.mCursorPosition.mLine = set_line-1;
						else
							m_pCurrentFile->m_editor->mState.mCursorPosition.mLine = set_line;
						m_pCurrentFile->m_editor->mState.mCursorPosition.mColumn = 0;
						m_pCurrentFile->m_editor->EnsureCursorVisible();
						m_pCurrentFile->m_editor->mScrollToCursor = true;
					}

				}
				/* old code
				for ( std::set<int>::iterator it = bmarks.begin(); it != bmarks.end(); ++it) {
					if (first < 0)
						first = *it;
					if (best < 0 && *it > mCurrentMark)
						best = *it;

				}
				if (best >= 0) {
					mCurrentMark = best;
				}
				else {
					//PE: TODO check for bookmarks in next file first.
					if (first >= 0) {
						mCurrentMark = first;
					}
				}

				//Goto mCurrentMark
				if (mCurrentMark > 0)
					mState.mCursorPosition.mLine = mCurrentMark-1;
				else
					mState.mCursorPosition.mLine = mCurrentMark;
				mState.mCursorPosition.mColumn = 0;
				EnsureCursorVisible();
				mScrollToCursor = true;
				*/
				lastKeySearch = (float)ImGui::GetTime();
			}
		}
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(188)) { // 188 = ,
			if (ImGui::GetTime() - lastKeySearch >= 0.125) {

				int first = -1, first_mark = 0;
				TextEditor * first_InsideEditor, *best_InsideEditor, *set_InsideEditor;
				int best = -1, best_mark = 0;

				extern int active_bookmark_list;
				extern int current_bookmark;

				set_InsideEditor = 0;

				int list_count = 0;

				current_bookmark--;
				if (current_bookmark < 0)
					current_bookmark = AllBookmark[active_bookmark_list].size() - 1;

				for (std::unordered_map<std::int32_t, BookmarkValues>::iterator it = AllBookmark[active_bookmark_list].begin(); it != AllBookmark[active_bookmark_list].end(); ++it)
				{

					if (first < 0) {
						first = it->second.lineno;
						first_InsideEditor = (TextEditor *)it->second.m_InsideEditor;
						first_mark = list_count;
					}
					if (list_count == current_bookmark) {
						best = it->second.lineno;
						best_InsideEditor = (TextEditor *)it->second.m_InsideEditor;
						best_mark = list_count;
					}

					list_count++;
				}

				int set_line = 0;
				if (best >= 0) {
					set_InsideEditor = best_InsideEditor;
					set_line = best;
				}
				else {
					if (first >= 0) {
						set_InsideEditor = first_InsideEditor;
						set_line = first;
					}
				}
				if (set_InsideEditor) {
					cProjectItem::sProjectFiles * m_pCurrentFile;
					m_pCurrentFile = FindFileFromEditorMatchSource(set_InsideEditor);
					if (m_pCurrentFile) {

						cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
						bFocusActiveEditor = true;
						if (set_line > 0)
							m_pCurrentFile->m_editor->mState.mCursorPosition.mLine = set_line - 1;
						else
							m_pCurrentFile->m_editor->mState.mCursorPosition.mLine = set_line;
						m_pCurrentFile->m_editor->mState.mCursorPosition.mColumn = 0;
						m_pCurrentFile->m_editor->EnsureCursorVisible();
						m_pCurrentFile->m_editor->mScrollToCursor = true;
					}
				}

				/* old code
				int last = -1;
				int best = -1;
				for (std::set<int>::iterator it = bmarks.begin(); it != bmarks.end(); ++it) {
					last = *it;
					if (*it < mCurrentMark)
						best = *it;

				}
				if (best >= 0) {
					mCurrentMark = best;
				}
				else {
					//PE: TODO check for bookmarks in prev file first.
					if (last >= 0) {
						mCurrentMark = last;
					}
				}
				//Goto mCurrentMark
				if (mCurrentMark > 0)
					mState.mCursorPosition.mLine = mCurrentMark - 1;
				else
					mState.mCursorPosition.mLine = mCurrentMark;
				mState.mCursorPosition.mColumn = 0;
				EnsureCursorVisible();
				mScrollToCursor = true;
				lastKeySearch = (float)ImGui::GetTime();
				*/
			}
		}
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(77)) { // 77 = m
			//BookMarks
			if (ImGui::GetTime() - lastKeySearch >= 0.125) {

//				if (bmarks.count(mState.mCursorPosition.mLine + 1) != 0)
				if (mBookMarks.count(mState.mCursorPosition.mLine + 1) != 0)
				{
					//Already there , remove.
//					bmarks.erase(mState.mCursorPosition.mLine + 1);
					mBookMarks.erase(mState.mCursorPosition.mLine + 1);
//					SetBookMarks(bmarks);
					SetBookMarks(mBookMarks);
				}
				else {
					//Not set , Add
//					bmarks.insert(mState.mCursorPosition.mLine + 1);
					mBookMarks.insert(mState.mCursorPosition.mLine + 1);
//					SetBookMarks(bmarks);
					SetBookMarks(mBookMarks);
				}
				lastKeySearch = (float)ImGui::GetTime();
			}
		}
		else if (ctrl==pref.bReplaceCtrl && shift==pref.bReplaceShift && alt==pref.bReplaceAlt && ImGui::IsKeyPressed(pref.iReplaceKey)) { // 0x48 = h
			bFreezeWord = false;
			if (ImGui::GetTime() - lastKeySearch >= 0.125) {
				Replace();
				lastKeySearch = (float)ImGui::GetTime();
			}
		}

		else if( ctrl==pref.bFindNextCtrl && shift==pref.bFindNextShift && alt==pref.bFindNextAlt && ImGui::IsKeyPressed(pref.iFindNextKey)) { // 0x72 = F3
			bFreezeWord = false;
			if (ImGui::GetTime() - lastKeySearch >= 0.125) {
				if (strlen(cSearch) > 0)
					FindNext();
				lastKeySearch = (float)ImGui::GetTime();
			}
		}
		else if (ImGui::IsKeyPressed(0x70)) { // 0x70 = F1
			bFreezeWord = false;
			if (ImGui::GetTime() - lastKeySearch >= 0.125) {
			
				if (pref.bBrowserHelp == false) {
					pref.bDisplayHelpWindow = true;
				}
				Help();

				lastKeySearch = (float)ImGui::GetTime();

			}
		}
		else if (ImGui::IsKeyPressed(0x78)) { // 0x78 = F9 , toggle breakpoint.

			if (ImGui::GetTime() - lastKeySearch >= 0.125) {
				if (bpts.count(mState.mCursorPosition.mLine + 1) != 0)
				{
					bpts.erase(mState.mCursorPosition.mLine + 1);
					SetBreakpoints(bpts);
					cProjectItem::sProjectFiles * m_pCurrentFile;
					m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
					if (m_pCurrentFile) {
						char tmp[MAX_PATH];
						sprintf(tmp, "delete breakpoint %s:%d", m_pCurrentFile->m_sName.GetStr(), mState.mCursorPosition.mLine + 1);
						uString bc_err;
						BroadcasterCommand(tmp,bc_err);
					}
				}
				else {
					bpts.insert(mState.mCursorPosition.mLine + 1);
					SetBreakpoints(bpts);
					cProjectItem::sProjectFiles * m_pCurrentFile;
					m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
					if (m_pCurrentFile) {
						char tmp[MAX_PATH];
						sprintf(tmp, "breakpoint %s:%d", m_pCurrentFile->m_sName.GetStr(), mState.mCursorPosition.mLine + 1);
						uString bc_err;
						BroadcasterCommand(tmp,bc_err);
					}
				}
				lastKeySearch = (float)ImGui::GetTime();
			}

		}
		else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow))) {

			timeStart = std::chrono::system_clock::now();
			//return system_clock::time_point(++s);

			if (bSuggestActice)
			{
				bCursorKeys = -1;
			}
			else
			{
				bEnableSuggest = false; //Cursor keys disable autocompleate until a new char is entered.
				bSuggestActice = false;
				MoveUp(1, shift);
			}
			bFreezeWord = false;
		}
		else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow))) {
			timeStart = std::chrono::system_clock::now();
			if (bSuggestActice)
			{
				bCursorKeys = 1;
			}
			else
			{
				bEnableSuggest = false; //Cursor keys disable autocompleate until a new char is entered.
				bSuggestActice = false;
				MoveDown(1, shift);
			}
			bFreezeWord = false;
		}

		//MAC Different.
		else if (keyboard_layout != 0 && is_osx && alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow))) {
			timeStart = std::chrono::system_clock::now();
			MoveLeft(1, shift, true);
			bEnableSuggest = false; //Cursor keys disable autocompleate until a new char is entered.
			bSuggestActice = false;
		}
		else if (keyboard_layout != 0 && is_osx && alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow))) {
			timeStart = std::chrono::system_clock::now();
			MoveRight(1, shift, true);
			bEnableSuggest = false; //Cursor keys disable autocompleate until a new char is entered.
			bSuggestActice = false;
		}
		else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow))) {
			timeStart = std::chrono::system_clock::now();
			if (ctrl && keyboard_layout != 0 && is_osx) {
				MoveHome(shift);
				bFreezeWord = false;
			}
			else {
				MoveLeft(1, shift, ctrl);
			}
			bEnableSuggest = false; //Cursor keys disable autocompleate until a new char is entered.
			bSuggestActice = false;
		}
		else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow))) {
			timeStart = std::chrono::system_clock::now();
			if (ctrl && keyboard_layout != 0 && is_osx) {
				MoveEnd(shift);
				bFreezeWord = false;
			}
			else {
				MoveRight(1, shift, ctrl);
			}
			bEnableSuggest = false; //Cursor keys disable autocompleate until a new char is entered.
			bSuggestActice = false;
		}
		

		else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageUp))) {
			MoveUp(GetPageSize() - 4, shift);
			bFreezeWord = false;
			bEnableSuggest = false; //Cursor keys disable autocompleate until a new char is entered.
		}
		else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageDown))) {
			MoveDown(GetPageSize() - 4, shift);
			bFreezeWord = false;
			bEnableSuggest = false; //Cursor keys disable autocompleate until a new char is entered.
		}
		else if (!alt && ctrl && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home))) {
			MoveTop(shift);
			bFreezeWord = false;
		}
		else if (ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End))) {
			MoveBottom(shift);
			bFreezeWord = false;
		}
		else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home))) {
			MoveHome(shift);
			bFreezeWord = false;
		}
		else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End))) {
			MoveEnd(shift);
			bFreezeWord = false;
		}
		else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
			Delete();
			bFreezeWord = false;
			mState.mSelectionStart = mState.mSelectionEnd = mState.mCursorPosition;
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;

		}
		else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace))) {
			BackSpace();
			bFreezeWord = false;
		}
		else if (!IsReadOnly() && !ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace))) {
			BackSpace();
			bFreezeWord = false;
		}
		else if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace))) {
			//Delete word to left.
			MoveLeft(1, true, true);
			BackSpace();
			mState.mSelectionStart = mState.mSelectionEnd = mState.mCursorPosition;
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;

			bFreezeWord = false;
		}
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
			mOverwrite ^= true;
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
			Copy();
		else if (!IsReadOnly() && !ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert))) {
			Paste();
			bFreezeWord = false;
		}
		else if (ctrl && !alt && ImGui::IsKeyPressed(32)) {
			bEnableFreezeInNextRun = true;
			bEnableSuggest = true;
		}
		//Only Mac and when using pc keyboard layout.
		//Emulate pc "insert" by using "delete" key instead. only for CTRL+SHIFT operations.
		else if (is_osx && keyboard_layout == 0  && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
			Copy();
		else if (is_osx && keyboard_layout == 0 && !IsReadOnly() && !ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
			Paste();
			bFreezeWord = false;
		}
		//
		else if (ctrl==pref.bCopyCtrl && shift==pref.bCopyShift && alt==pref.bCopyAlt && ImGui::IsKeyPressed(pref.iCopyKey))
			Copy();
		else if (!IsReadOnly() && ctrl==pref.bPasteCtrl && shift==pref.bPasteShift && alt==pref.bPasteAlt && ImGui::IsKeyPressed(pref.iPasteKey)) {
			Paste();
			bFreezeWord = false;
		}
		else if (ctrl==pref.bCutCtrl && shift==pref.bCutShift && alt==pref.bCutAlt && ImGui::IsKeyPressed(pref.iCutKey)) {
			Cut();
			bFreezeWord = false;
			mState.mSelectionStart = mState.mSelectionEnd = mState.mCursorPosition;
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		}
		else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
			Cut();
			bFreezeWord = false;
			mState.mSelectionStart = mState.mSelectionEnd = mState.mCursorPosition;
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		}
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A))) {
			SelectAll();
			bFreezeWord = false;
		}
		else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
				
			if (bSuggestActice && !bLookupReadOnly)
				bTabPressed = true;	//Reuse tabpressed its the same as return.
			else
				EnterCharacter('\n', false);
			bFreezeWord = false;
		}
		else if (!IsReadOnly() )
		{
			//io.AddInputCharacter()
			//InputCharacters renamed to InputQueueCharacters
			//if (io.InputCharacters[0])
			if (io.InputQueueCharacters.size() > 0 ) // io.InputQueueCharacters[0]
			{
				// Process text input (before we check for Return because using some IME will effectively send a Return?)
				// We ignore CTRL inputs, but need to allow ALT+CTRL as some keyboards (e.g. German) use AltGR (which _is_ Alt+Ctrl) to input certain characters.
				bEnableSuggest = true;
				bool ignore_inputs = (io.KeyCtrl && !io.KeyAlt) || (is_osx && io.KeySuper );
				if (!ignore_inputs )
					//for (int n = 0; n < IM_ARRAYSIZE(io.InputCharacters) && io.InputCharacters[n]; n++)
					for (int n = 0; n < io.InputQueueCharacters.size()  && io.InputQueueCharacters[n]; n++)
					{
						//UTF8: Need support here. set glypth mInt.

						// Insert character if they pass filtering
						unsigned int c = (unsigned int)io.InputQueueCharacters[n];

						//Emulate UTF8 by added multiply bytes.
						if (c == '(')
							bFreezeWord = true;
						if (c == ')')
							bFreezeWord = false;
						if (c == '\t' && bSuggestActice && !bLookupReadOnly) {
							bTabPressed = true;
						}
						else {

							EnterCharacter((char)c, shift, c);
						}
//						ImWchar ch = c;
//						const int new_text_len_utf8 = ImTextCountUtf8BytesFromStr(&ch, &ch + 1);
//						memcpy(tmp, &ch, (size_t)1 * sizeof(ImWchar));
//						agk::Print("");
//						if (InputTextFilterCharacter(&c, flags, callback, callback_user_data))
//							edit_state.OnKeyPressed((int)c);
					}

				// Consume characters
//				memset(g.IO.InputCharacters, 0, sizeof(g.IO.InputCharacters));
			}
		}

/*
		else if (!IsReadOnly() && !ctrl && !alt)
		{



			for (size_t i = 0; i < sizeof(io.InputCharacters) / sizeof(io.InputCharacters[0]); i++)
			{
				auto c = (unsigned char)io.InputCharacters[i];
				if (c != 0)
				{
					if (isprint(c) || isspace(c))
					{
						EnterCharacter((char)c, shift);
					}
				}
			}
		}
*/
	}

	//Auto compleate.
	/*
	if (ImGui::IsWindowFocused()) {
		//Use last key time if more then 10 mil not constant typing , look up keywords.
		agk::Print(lastKeyedWord);
	}
	*/


	/*
		Mouse inputs
	*/

	if (ImGui::IsWindowHovered() )
	{
		static float lastClick = -1.0f;
		if (!alt) //!shift
		{
			auto click       = ImGui::IsMouseClicked(0);
			//auto rclick		 = ImGui::IsMouseClicked(1);
			auto doubleClick = ImGui::IsMouseDoubleClicked(0);
			auto t           = ImGui::GetTime();
			auto tripleClick = click && !doubleClick && t - lastClick < io.MouseDoubleClickTime;

			/*
				Left mouse button triple click
			*/

			if (tripleClick)
			{
				if (!ctrl)
				{
					bEnableSuggest = false;
					bSuggestActice = false;
					mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = SanitizeCoordinates(ScreenPosToCoordinates(ImGui::GetMousePos(),true));
					mSelectionMode = SelectionMode::Line;
					SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);
				}

				lastClick = -1.0f;
			}

			/*
				Left mouse button double click
			*/

			else if (doubleClick)
			{
				if (!ctrl)
				{
					bEnableSuggest = false;
					bSuggestActice = false;
					mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = SanitizeCoordinates(ScreenPosToCoordinates(ImGui::GetMousePos(), true));
					if (mSelectionMode == SelectionMode::Line)
						mSelectionMode = SelectionMode::Normal;
					else
						mSelectionMode = SelectionMode::Word;
					SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode,true);
					if (mSelectionMode == SelectionMode::Word)
						mInteractiveStart = mState.mSelectionStart;
				}

				lastClick = (float)ImGui::GetTime();
			}

			/*
				Left mouse button click
			*/

			else if (click)
			{
				bEnableSuggest = false;
				bSuggestActice = false;
				Coordinates bpc = ScreenPosToCoordinates(ImGui::GetMousePos(), true);

				ImVec2 origin = ImGui::GetCursorScreenPos();
				ImVec2 local(ImGui::GetMousePos().x - origin.x, ImGui::GetMousePos().y - origin.y);

				if (shift)
					mState.mCursorPosition = mInteractiveEnd = SanitizeCoordinates(bpc);
				else
					mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = SanitizeCoordinates(bpc);

				if (local.x <= TOGGLEBREAKPOINTX ) // click in margin , enable disable breakpoints.
				{
					if (isAGKsource) {

						if (bpts.count(mState.mCursorPosition.mLine + 1) != 0)
						{
							//Already there , remove.
							//bpts.find()

							bpts.erase(mState.mCursorPosition.mLine + 1);
							SetBreakpoints(bpts);

							cProjectItem::sProjectFiles * m_pCurrentFile;
							m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
							if (m_pCurrentFile) {
								char tmp[MAX_PATH];
								sprintf(tmp, "delete breakpoint %s:%d", m_pCurrentFile->m_sName.GetStr(), mState.mCursorPosition.mLine + 1);
								uString bc_err;
								BroadcasterCommand(tmp,bc_err);
							}
							//addAllBreakPoints(NULL);
						}
						else {
							//Not set , Add
							//bpts.clear();
							bpts.insert(mState.mCursorPosition.mLine + 1);
							SetBreakpoints(bpts);
							cProjectItem::sProjectFiles * m_pCurrentFile;
							m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
							if (m_pCurrentFile) {
								char tmp[MAX_PATH];
								sprintf(tmp, "breakpoint %s:%d", m_pCurrentFile->m_sName.GetStr(), mState.mCursorPosition.mLine + 1);
								uString bc_err;
								BroadcasterCommand(tmp,bc_err);
							}
							//addAllBreakPoints(NULL);
						}
					}
				}

				if (ctrl)
					mSelectionMode = SelectionMode::Word;
				else
					mSelectionMode = SelectionMode::Normal;

				SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode,true);
				ImGui::SetWindowFocus();
				lastClick = (float)ImGui::GetTime();
				//EnsureCursorVisible();
			}
			// Mouse left button dragging (=> update selection)
			else if (ImGui::IsMouseDragging(0) && ImGui::IsMouseDown(0))
			{
				io.WantCaptureMouse = true;
				mState.mCursorPosition = mInteractiveEnd = SanitizeCoordinates(ScreenPosToCoordinates(ImGui::GetMousePos(), true));
				SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);
				EnsureCursorVisible();
			}

		}
	}
	//fps 1100

	static std::string buffer;
	auto contentSize = ImGui::GetWindowContentRegionMax();
	auto drawList = ImGui::GetWindowDrawList();
	float longest(mTextStart);

	ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
	auto scrollX = ImGui::GetScrollX();
	auto scrollY = ImGui::GetScrollY();

	auto lineNo = (int)floor(scrollY / mCharAdvance.y);
	auto scLineNo = (int)floor(scrollY / mCharAdvance.y);

	//PE: Line conversion list needed.
	//so lineNo can be used directly.

	iFoldingLines = 0;
	iFoldeStarters = 0;
	bInFolding = false;
	bFoldNow = false;
	breakonnext = false;
	int iFolderPossible = 0;

	//PE: code folding.
	if ( pref.iEnableCodeFolding) {
		//PE: This need a speed test in very large files.
		int newline = 0;

		#define MAXFOLDERCHECKSPERSYNC 5000
		if (iFoldingCheckStart >= mLines.size() || iFoldingCheckStart < 0 ) {
			iFoldingCheckStart = 0;
			iFoldingCheckCount = 0;
			iHiddenLinesNew = iHiddenLines;
			iHiddenLines = 0;
		}
		if (iFoldingCheckStart > 0)
			newline = m_iNewLineNo[iFoldingCheckStart - 1]+1;

		iFoldingCheckCount = 0;

		for (int i = iFoldingCheckStart; i <= mLines.size() ; i++) {

			if (!bFoldNow && iFoldeStarters == 0 && iFolderPossible == 0 && iFoldingCheckCount++ > MAXFOLDERCHECKSPERSYNC ) {
				break;
			}
			iFoldingCheckStart++;

			if (i < MAXPRECHECKLINES) {
				if (newline > MAXPRECHECKLINES - 1)
					newline = MAXPRECHECKLINES - 1;
				m_iNewLineNo[i] = newline;
				m_iHiddenLineNo[newline] = i;
				if (!m_bLineVisible[newline] ) {
					while (!m_bLineVisible[newline]) {
						if (newline >= mLines.size()-1)
							break;
						iHiddenLines++;
						newline++;
						if (newline > MAXPRECHECKLINES - 1) {
							newline = MAXPRECHECKLINES - 1;
							break;
						}
						m_iHiddenLineNo[newline] = i;
					}
				}
//				m_iHiddenLineNo[i] = i - iHiddenLines;
				if (newline < mLines.size())
					newline++;

				if (m_bFoldingStartWord[i]) {
					if (iFoldeStarters < 0) iFoldeStarters = 0;
					iFoldeStarters++;
					if (iFolderPossible < 0) iFolderPossible = 0;
					iFolderPossible++;
					m_bLineVisible[i] = true; //This must always be visible.
				}

				if (iFolderPossible > 0 && m_bLineVisible[i])
					m_bFoldingPossible[i] = true;
				else {
					m_bFoldingPossible[i] = false;
				}

				if (m_bFoldingEndWord[i]) {
					if (iFoldeStarters > 0)
						iFoldeStarters--;
					if(iFolderPossible > 0)
						iFolderPossible--;
				}

				if (m_bFoldingStartWord[i])
					m_bFoldingPossible[i] = false;


				if (bFoldNow) {
					m_bLineVisible[i] = false;
					if ((m_bFoldingEndWord[i] || m_bFoldingStartWord[i]) && (iFoldeStarters <= 0 || breakonnext)) {
						bFoldNow = false;
						breakonnext = false;
					}
				}
				else {
					m_bLineVisible[i] = true;
				}


				if (mFoldingActive.count(i + 1) != 0) {

					//Need a direct update.
					ColorizeRange(i, i + 1);
					if (m_bFoldingStartWord[i]) {

						if (!bFoldNow)
							iFoldeStarters = 1;

						bFoldNow = true;
					}
					else {
						//mFoldingActive and m_bFoldingStartWord[i] dont match , delete ?
						mFoldingActive.erase(i + 1);
					}

				}

				if (newline > mLines.size() - 1) {
					m_iNewLineNo[i] = mLines.size() - 1;
					//m_iHiddenLineNo[i] = mLines.size() - iHiddenLines;
				}
			}
		}

		if( ((int)floor(scrollY / mCharAdvance.y)) < MAXPRECHECKLINES )
			lineNo = m_iNewLineNo[(int)floor(scrollY / mCharAdvance.y)];
	}

	auto globalLineMax = (int)mLines.size();
	auto lineMax = std::max(0, std::min((int)mLines.size() - 1, lineNo + (int)floor((scrollY + contentSize.y) / mCharAdvance.y)));

	ColorizeInternal(lineNo, lineMax);

	//#######################
	//#### Search Dialog ####
	//#######################

	if (wtabvisible && bSearchActive ) {
		ImVec2 oldCursor = ImGui::GetCursorPos();
		//ImGui::SetNextWindowSize(ImVec2(300, 20));
//		ImGui::SetCursorPos(ImVec2( ImGui::GetWindowSize().x - 300, 0));
		ImVec2 SearchDialogSize = ImVec2(59* fontSize, 24 + fontSize);//420
		if (bFindNextEndOfLine || bFindPrevStartOfLine || strlen(ReplaceMsg) > 0)
			SearchDialogSize.y += ( (fontSize*3.1) );
		if (replaceactive) {
			if (bHideReplaceBut) {
				SearchDialogSize.x += ((fontSize * 9));
				SearchDialogSize.y += ((fontSize * 13 + 28));
			}
			else
				SearchDialogSize.y += ((fontSize * 6 + 28));
		}

		ImVec4* style_colors = ImGui::GetStyle().Colors;
		ImVec2 window_pos = ImGui::GetWindowPos();
		window_pos.x += (ImGui::GetWindowSize().x - SearchDialogSize.x );
		ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX()+(ImGui::GetWindowSize().x - SearchDialogSize.x), ImGui::GetScrollY() ));

		ImGui::BeginChild("##SearchFindReplaceDialog", ImVec2(SearchDialogSize.x- ImGui::GetCurrentWindow()->ScrollbarSizes.x, SearchDialogSize.y), aBorder, ImGuiWindowFlags_NoMove);

		ImGui::GetWindowDrawList()->AddRectFilled(window_pos - ImVec2(2, 2), window_pos + SearchDialogSize, ImGui::GetColorU32(ImVec4(style_colors[ImGuiCol_MenuBarBg])));
//		ImGui::PushItemWidth(SearchDialogSize.x-70.0 - (fontSize*5));
		ImGui::PushItemWidth(SearchDialogSize.x - 70.0 - (fontSize * 13)); //8
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10, ImGui::GetCursorPos().y));

		//Make sure we get focus first time.
		static int focustimes = 0;
		if (bSetInputFocus) {
			ImGui::SetKeyboardFocusHere();
			//ImGui::SetWindowFocus();
			//ImGui::SetNextWindowFocus();
			if (focustimes++ > 2) {
				bSetInputFocus = false;
				focustimes = 0;
//				strcpy(ReplaceMsg, "");
			}

		}
//		else {
//			if ( ( ctrl && !shift && !alt && ImGui::IsKeyPressed(0x46) ) || ImGui::IsKeyPressed(27 ) ) { // 0x46 = f , 27 = esc.
//				ToggleSeachReplace();
//			}
//		}
		//ImGui::SetKeyboardFocusHere();
		if( wtabvisible && ImGui::GetTime() - lastKeySearch >= 0.125 ) { //small delay before active.
			if( ctrl==pref.bFindCtrl && shift==pref.bFindShift && alt==pref.bFindAlt && ImGui::IsKeyPressed(pref.iFindKey) ) { // 0x46 = f , 27 = esc.
				ToggleSeachReplace(false);
				lastKeySearch = (float)ImGui::GetTime();
				bSetInputFocus = true;
				bFreezeWord = false;
				replaceactive = false;
			}

			if (ctrl == pref.bReplaceCtrl && shift == pref.bReplaceShift && alt == pref.bReplaceAlt && ImGui::IsKeyPressed(pref.iReplaceKey)) { // 0x48 = h
				bFreezeWord = false;
				if (ImGui::GetTime() - lastKeySearch >= 0.125) {
					strcpy(ReplaceMsg, "");
					bUndoAll = 0;
					Replace();
					lastKeySearch = (float)ImGui::GetTime();
				}
			}

			if (!IsReadOnly() && ctrl && shift && !alt && ImGui::IsKeyPressed(0x46)) { // 0x46 = f
				//CTRL+SHIFT+F bring replace dialog up, include advanced search.
				bFreezeWord = false;
				if (ImGui::GetTime() - lastKeySearch >= 0.125) {
					strcpy(ReplaceMsg, "");
					bUndoAll = 0;
					Replace();
					bHideReplaceBut = true;
					lastKeySearch = (float)ImGui::GetTime();
				}

			}

			if( ImGui::IsKeyPressed(27) )
			{
				ToggleSeachReplace(true);
			}
			if (ctrl == pref.bFindNextCtrl && shift == pref.bFindNextShift && alt == pref.bFindNextAlt && ImGui::IsKeyPressed(pref.iFindNextKey)) { // 0x72 = F3
				if (strlen(cSearch) > 0) {
					strcpy(ReplaceMsg, "");
					bUndoAll = 0;
					ToggleSeachReplace(false, true); //force add current to list.
					FindNext();
				}
				lastKeySearch = (float)ImGui::GetTime();
			}
		}

		if (ImGui::InputText("##f", &cSearch[0], MAX_PATH, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
			//Return pressed , start search from current location.
			strcpy(cnSearch, cSearch);
			strcpy(ReplaceMsg, "");
			bUndoAll = 0;
			ToggleSeachReplace(false, true); //force add current to list.
			FindNext();
			bSetInputFocus = true; //get focus back.
		}
		strcpy(cnSearch, cSearch);

		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (!search_items[0].GetStr())
			search_items[0] = "";
		static char * current_combo_search = (char*) search_items[0].GetStr();
		int comboflags = ImGuiComboFlags_NoPreview;
		if (ImGui::BeginCombo("##combolastsearch", current_combo_search , comboflags))
		{
			for (int n = 0; n < combo_search_entrys; n++)
			{
				if (!search_items[n].GetStr())
					search_items[n] = "";
				bool is_selected = (current_combo_search == search_items[n].GetStr());
				if (ImGui::Selectable(search_items[n].GetStr(), is_selected)) {
					current_combo_search = (char *) search_items[n].GetStr();
					char cTmpSearch[MAX_PATH];
					strcpy(cTmpSearch, search_items[n].GetStr());
					ToggleSeachReplace(false,true); //force add current to list.
					strcpy( cSearch , cTmpSearch);
					strcpy(ReplaceMsg, "");
					bUndoAll = 0;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 4, ImGui::GetCursorPos().y));
		if (ImGui::Button("<") || iNextFindDialogAction == 9 ) {
			iNextFindDialogAction = -1;
			strcpy(ReplaceMsg, "");
			bUndoAll = 0;
			ToggleSeachReplace(false, true); //force add current to list.
			FindPrev();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Find Previous.");

		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 4, ImGui::GetCursorPos().y));
		if (ImGui::Button(">") || iNextFindDialogAction == 0 ) {
			iNextFindDialogAction = -1;
			strcpy(ReplaceMsg, "");
			bUndoAll = 0;
			ToggleSeachReplace(false, true); //force add current to list.
			FindNext();
			bSetInputFocus = true; //get focus back.
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Find Next.");

		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 4, ImGui::GetCursorPos().y));
		if (ImGui::Button("a") || iNextFindDialogAction == 1 ) {
			iNextFindDialogAction = -1;
			//AddToCompilerLog

			strcpy(ReplaceMsg, "");
			bUndoAll = 0;
			if (pCurrentSelectedProject) {
				ClearAllEditorMarkers();
				ClearCompilerLog();
				cProjectItem::sProjectFiles * allProjectFiles;
				cProjectItem * allMyProjects;

				allMyProjects = allProjects;
				while (allMyProjects) {
					allProjectFiles = allMyProjects->m_pFirstFile;
					while (allProjectFiles) {
						if (allProjectFiles->m_editor == m_ActiveEditor) {



							//The current editor.
							Coordinates oldpos = mState.mCursorPosition;
							//Start find all at the top.
							mState.mCursorPosition.mLine = 0;
							mState.mCursorPosition.mColumn = 0;

							bFindNextEndOfLine = false;
							int replaced = 0;
							while (!bFindNextEndOfLine) {

								int oldLine = mState.mCursorPosition.mLine;
								int oldCol = mState.mCursorPosition.mColumn;

								FindNext();
								if (!bFindNextEndOfLine && HasSelection()) {
									std::string gst;
									auto oldSelEnd = mState.mSelectionEnd;
									auto oldSelStart = mState.mSelectionStart;

									const auto& line = mLines[mState.mSelectionStart.mLine];
									mState.mSelectionStart.mColumn = 0;
									mState.mSelectionEnd.mColumn = line.size();
									gst = GetSelectedText();

									mState.mSelectionEnd = oldSelEnd;
									mState.mSelectionStart = oldSelStart;

									//We had double parsing of strings so content inside code (like "%33332234d") could generate errors.
									//fixed but keep try catch just in case.
									try {
										//Trim gst.
										// trim leading spaces
										size_t startpos = gst.find_first_not_of(" \t");
										if (std::string::npos != startpos)
										{
											gst = gst.substr(startpos);
										}

										AddToCompilerLog(0, "%s:%d:Search:%d:%d: %s\n", allProjectFiles->m_sName.GetStr(), mState.mSelectionStart.mLine + 1, mState.mSelectionStart.mColumn, mState.mSelectionEnd.mColumn, gst.c_str());
										replaced++;
									}
									catch (std::exception & e)
									{
										//Ignore exceptions.
									}

								}

								//PE: bug, if findnext dont set a new cursor position something went wrong, we need to check this.
								if (oldLine == mState.mCursorPosition.mLine && oldCol == mState.mCursorPosition.mColumn) {
									//Something went wrong. goto next line.
									mState.mCursorPosition.mLine++;
								}

							}

							bool bt = m_ActiveEditor->bFindNextEndOfLine;
							if (replaced > 0) {
								//SetFocus.
								ImGui::SetWindowFocus("Message window");
								error_log_scroll_to_top();
							}
							sprintf(ReplaceMsg, "Found %d times.", replaced);
							bFindNextEndOfLine = false;
							bFindPrevStartOfLine = false;
							mState.mCursorPosition = oldpos;


						}
						allProjectFiles = allProjectFiles->m_pNext;
					}

					allMyProjects = allMyProjects->m_pNext;
				}
			}

			bSetInputFocus = true; //get focus back.
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Find All.");


		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 4, ImGui::GetCursorPos().y));
		if (ImGui::Button("s") || iNextFindDialogAction == 2 ) {
			iNextFindDialogAction = -1;
			//AddToCompilerLog
			strcpy(ReplaceMsg, "");
			bUndoAll = 0;
			if (pCurrentSelectedProject) {
				ClearAllEditorMarkers();
				ClearCompilerLog();
				cProjectItem::sProjectFiles * allProjectFiles;
				cProjectItem * allMyProjects;

				allMyProjects = allProjects;
				while (allMyProjects) {
					allProjectFiles = allMyProjects->m_pFirstFile;
					while (allProjectFiles) {
						if (allProjectFiles->m_editor == m_ActiveEditor) {


							Coordinates oldpos = mState.mCursorPosition;
							//Start replace all at the top.
							mState.mCursorPosition.mLine = 0;
							mState.mCursorPosition.mColumn = 0;
							bFindNextEndOfLine = false;

							auto oldSelEnd = mState.mSelectionEnd;
							auto oldSelStart = mState.mSelectionStart;

							//mState.mCursorPosition = mState.mSelectionEnd;

							int replaced = 0;
							while (!bFindNextEndOfLine) {

								int oldLine = mState.mCursorPosition.mLine;
								int oldCol = mState.mCursorPosition.mColumn;

								FindNext();

								if (HasSelection()) {
									if (mState.mSelectionStart >= oldSelStart && mState.mSelectionEnd <= oldSelEnd) {
										std::string gst;
										auto moldSelEnd = mState.mSelectionEnd;
										auto moldSelStart = mState.mSelectionStart;

										const auto& line = mLines[mState.mSelectionStart.mLine];
										mState.mSelectionStart.mColumn = 0;
										mState.mSelectionEnd.mColumn = line.size();
										gst = GetSelectedText();

										mState.mSelectionEnd = moldSelEnd;
										mState.mSelectionStart = moldSelStart;
										
										try {
											size_t startpos = gst.find_first_not_of(" \t");
											if (std::string::npos != startpos)
											{
												gst = gst.substr(startpos);
											}

											AddToCompilerLog(0, "%s:%d:Search:%d:%d: %s\n", allProjectFiles->m_sName.GetStr(), mState.mSelectionStart.mLine + 1, mState.mSelectionStart.mColumn, mState.mSelectionEnd.mColumn, gst.c_str());
											replaced++;
										}
										catch (std::exception & e)
										{
											//Ignore exceptions.
										}

									}
								}

								//PE: bug, if findnext dont set a new cursor position something went wrong, we need to check this.
								if (oldLine == mState.mCursorPosition.mLine && oldCol == mState.mCursorPosition.mColumn) {
									//Something went wrong. goto next line.
									mState.mCursorPosition.mLine++;
								}

							}

							mState.mSelectionEnd = oldSelEnd;
							mState.mSelectionStart = oldSelStart;

							if (replaced > 0) {
								ImGui::SetWindowFocus("Message window");
								error_log_scroll_to_top();
							}
							sprintf(ReplaceMsg, "Found %d times.", replaced);
							bFindNextEndOfLine = false;
							bFindPrevStartOfLine = false;
							mState.mCursorPosition = oldpos;

							//bSetInputFocus = true; //get focus back.


						}
						allProjectFiles = allProjectFiles->m_pNext;
					}

					allMyProjects = allMyProjects->m_pNext;
				}
			}

		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Find in Selection.");



		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 4, ImGui::GetCursorPos().y));
		if (ImGui::Button("p") || iNextFindDialogAction == 3 ) {

			iNextFindDialogAction = -1;

			//AddToCompilerLog

			strcpy(ReplaceMsg, "");
			bUndoAll = 0;
			if (pCurrentSelectedProject) {
				char firstentry[1024];
				ClearAllEditorMarkers();
				ClearCompilerLog();
				cProjectItem::sProjectFiles * allProjectFiles;
				cProjectItem * allMyProjects;
				int replaced = 0;
				allMyProjects = allProjects;
				while (allMyProjects) {
					allProjectFiles = allMyProjects->m_pFirstFile;
					while (allProjectFiles) {

						if (allMyProjects == pCurrentSelectedProject) {

							bool justloaded = false;
							if (!allProjectFiles->m_bOpenForEdit ) {
								//We need to open it for replace to work.
								strcpy(cnSearch, cSearch);
								strcpy(cnSearchReplace, cSearchReplace);
								vTextEditorInit(allProjectFiles->m_editor, (char *)allProjectFiles->m_sFullPath.GetStr());
								allProjectFiles->m_bOpenForEdit = true;
								justloaded = true;
								strcpy(cSearch, cnSearch);
								strcpy(cSearchReplace, cnSearchReplace);
							}
							int tmpreplaced = replaced;

							if (allProjectFiles->m_bOpenForEdit && allProjectFiles->m_editor && !allProjectFiles->m_scene ) {


								Coordinates oldpos = allProjectFiles->m_editor->mState.mCursorPosition;
								//Start replace all at the top.
								allProjectFiles->m_editor->mState.mCursorPosition.mLine = 0;
								allProjectFiles->m_editor->mState.mCursorPosition.mColumn = 0;

								allProjectFiles->m_editor->bFindNextEndOfLine = false;
								
								strcpy(allProjectFiles->m_editor->cnSearch, cSearch);
								
								while (!allProjectFiles->m_editor->bFindNextEndOfLine) {

									//mState.mCursorPosition.mLine++

									int oldLine = allProjectFiles->m_editor->mState.mCursorPosition.mLine;
									int oldCol = allProjectFiles->m_editor->mState.mCursorPosition.mColumn;

									allProjectFiles->m_editor->FindNext();
									if (!allProjectFiles->m_editor->bFindNextEndOfLine && allProjectFiles->m_editor->HasSelection()) {

										std::string gst;
										auto oldSelEnd = allProjectFiles->m_editor->mState.mSelectionEnd;
										auto oldSelStart = allProjectFiles->m_editor->mState.mSelectionStart;

										const auto& line = allProjectFiles->m_editor->mLines[allProjectFiles->m_editor->mState.mSelectionStart.mLine];
										allProjectFiles->m_editor->mState.mSelectionStart.mColumn = 0;
										allProjectFiles->m_editor->mState.mSelectionEnd.mColumn = line.size();
										gst = allProjectFiles->m_editor->GetSelectedText();

										allProjectFiles->m_editor->mState.mSelectionEnd = oldSelEnd;
										allProjectFiles->m_editor->mState.mSelectionStart = oldSelStart;

										//Strange exeption on Mike system when search all projects.
										//Invalid integer length modifier.
										//Must be around here.
										//Found the problem in "AddLog()" function , double parsing of %.
										//Keep try catch here anyway just in case.
										try
										{
											// trim leading spaces
											size_t startpos = gst.find_first_not_of(" \t");
											if (std::string::npos != startpos)
											{
												gst = gst.substr(startpos);
											}

											AddToCompilerLog(0, "%s:%d:Search:%d:%d: %s\n", allProjectFiles->m_sName.GetStr(), allProjectFiles->m_editor->mState.mSelectionStart.mLine + 1, allProjectFiles->m_editor->mState.mSelectionStart.mColumn, allProjectFiles->m_editor->mState.mSelectionEnd.mColumn, gst.c_str());
											if (replaced == 0) {
												sprintf(firstentry, "%s:%d:Search:%d:%d: %s\n", allProjectFiles->m_sName.GetStr(), allProjectFiles->m_editor->mState.mSelectionStart.mLine + 1, allProjectFiles->m_editor->mState.mSelectionStart.mColumn, allProjectFiles->m_editor->mState.mSelectionEnd.mColumn, gst.c_str());
											}
											replaced++;

										}
										catch (std::exception & e)
										{
											//Ignore exceptions.
										}

										//PE: bug, if findnext dont set a new cursor position something went wrong, we need to check this.
										if (oldLine == allProjectFiles->m_editor->mState.mCursorPosition.mLine && oldCol == allProjectFiles->m_editor->mState.mCursorPosition.mColumn) {
											//Something went wrong. goto next line.
											allProjectFiles->m_editor->mState.mCursorPosition.mLine++;
										}
										

									}

									//PE: bug, if findnext dont set a new cursor position something went wrong, we need to check this.
									if (oldLine == allProjectFiles->m_editor->mState.mCursorPosition.mLine && oldCol == allProjectFiles->m_editor->mState.mCursorPosition.mColumn) {
										//Something went wrong. goto next line.
										allProjectFiles->m_editor->mState.mCursorPosition.mLine++;
									}

								}

								allProjectFiles->m_editor->bFindNextEndOfLine = false;
								allProjectFiles->m_editor->mState.mCursorPosition = oldpos;

							}

							if (justloaded && tmpreplaced == replaced) {
								//Just close it again. nothing was found.
								allProjectFiles->m_editor->quiteditor = true;
							}

						}
						allProjectFiles = allProjectFiles->m_pNext;
					}

					allMyProjects = allMyProjects->m_pNext;
				}

				if (replaced > 0) {
					//SetFocus.
					ImGui::SetWindowFocus("Message window");
					ParseCompilerLog(firstentry, false);
					error_log_scroll_to_top();
				}
				sprintf(ReplaceMsg, "Found %d times.", replaced);
				bFindNextEndOfLine = false;
				bFindPrevStartOfLine = false;

			}

			bSetInputFocus = true; //get focus back.
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Find in All Project Files.");




		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 4, ImGui::GetCursorPos().y));
		if (ImGui::Button("X")) {
			ToggleSeachReplace(true);
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Close.");


		if (replaceactive && bHideReplaceBut) {

			//PE: SHIFT+F find dialog.
			ImGui::Columns(2, "myShiftFindDialog", false);  // 3-ways, no border
			ImGui::Text("");
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10, ImGui::GetCursorPos().y));
			ImGui::RadioButton("Find##1shiftfdialog", &iFindDialogAction, 0);
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10, ImGui::GetCursorPos().y));
			ImGui::RadioButton("Find All##2shiftfdialog", &iFindDialogAction, 1);
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10, ImGui::GetCursorPos().y));
			ImGui::RadioButton("Find in Selection##3shiftfdialog", &iFindDialogAction, 2);
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10, ImGui::GetCursorPos().y));
			ImGui::RadioButton("Find in All Project Files##4shiftfdialog", &iFindDialogAction, 3);


			ImGui::NextColumn();
			ImGui::Text("");
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 20, ImGui::GetCursorPos().y));
			ImGui::Checkbox("Match case##5shiftfdialog", &bMatchCase);
			ImGui::Text("");
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 20, ImGui::GetCursorPos().y));
			ImGui::Checkbox("Word only##6shiftfdialog", &bMatchWord);
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 20, ImGui::GetCursorPos().y +(mCharAdvance.y*0.5)  ));
			
			if (iFindDialogAction == 0) {
				if (ImGui::Button("Find Previous##myprevshiftfdialog")) {
					iNextFindDialogAction = 9;
				}
				ImGui::SameLine();
				ImGui::Text(" ");
				ImGui::SameLine();
				if (ImGui::Button("Find Next##mynextshiftfdialog")) {
					iNextFindDialogAction = iFindDialogAction;
				}
			}
			else {
				if (ImGui::Button("Find##myshiftfdialog")) {
					iNextFindDialogAction = iFindDialogAction;
				}
			}
			ImGui::Columns(1);
			ImGui::Text("");
		}
		else if (replaceactive)
		{
			ImGui::PushItemWidth(SearchDialogSize.x - 100.0 - (fontSize * 7));
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10, ImGui::GetCursorPos().y));
			if (ImGui::InputText("Replace with", &cSearchReplace[0], MAX_PATH, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
				//Return pressed , start search from current location.
				strcpy(ReplaceMsg, "");
				bUndoAll = 0;
				FindNext();
				bSetInputFocus = true; //get focus back.
			}
			strcpy(cnSearchReplace, cSearchReplace);

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10, ImGui::GetCursorPos().y));
			if (ImGui::Button("Replace")) {
				strcpy(ReplaceMsg, "");
				bUndoAll = 0;
				if (HasSelection()) {
					if( strlen(cSearchReplace) > 0 )
						PasteString( cSearchReplace );
					else
						Cut();
					//Cut , PasteString set cursor after insert so must go one back.
					Coordinates pos = GetActualCursorCoordinates();
					if (pos.mColumn > 0) pos.mColumn--;
					SetCursorPosition(pos);
					FindNext();
				}
				else {
					FindNext();
				}
				//bFindNextEndOfLine

			}
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 6, ImGui::GetCursorPos().y));
			if (ImGui::Button("Replace All")) {

				Coordinates oldpos = mState.mCursorPosition;
				//Start replace all at the top.
				mState.mCursorPosition.mLine = 0;
				mState.mCursorPosition.mColumn=0;
				bFindNextEndOfLine = false;
				strcpy(ReplaceMsg, "");
				bUndoAll = 0;
				int replaced = 0;
				while (!bFindNextEndOfLine) {
					FindNext();
					if (HasSelection()) {
						//Coordinates beforecut = mState.mSelectionEnd;
						if (strlen(cSearchReplace) > 0)
							PasteString(cSearchReplace);
						else
							Cut();

						//Cut , PasteString set cursor after insert so must go one back.
						Coordinates pos = GetActualCursorCoordinates();
						if (pos.mColumn > 0) pos.mColumn--;
						//SetCursorPosition(beforecut);
						SetCursorPosition(pos);

						replaced++;
					}
				}
				bool bt = m_ActiveEditor->bFindNextEndOfLine;
				//m_ActiveEditor->cSearch;

				sprintf(ReplaceMsg, "Found and replaced %d times.", replaced);
				bUndoAll = replaced;
				bFindNextEndOfLine = false;
				bFindPrevStartOfLine = false;
				mState.mCursorPosition = oldpos;
			}
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 6, ImGui::GetCursorPos().y));
			ImGui::Checkbox("Match case ", &bMatchCase);
			ImGui::SameLine();
			ImGui::Checkbox("Word only", &bMatchWord);

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 10, ImGui::GetCursorPos().y+2));

			if (pCurrentSelectedProject) {
				if (ImGui::Button("In All Project Files")) {

					int replaced = 0;
					cProjectItem::sProjectFiles * allProjectFiles;
					allProjectFiles = pCurrentSelectedProject->m_pFirstFile;
					while (allProjectFiles) {
						bool justloaded = false;
						if (!allProjectFiles->m_bOpenForEdit) {
							//We need to open it for replace to work.
							strcpy(cnSearch,cSearch);
							strcpy(cnSearchReplace, cSearchReplace);
							vTextEditorInit(allProjectFiles->m_editor, (char *)allProjectFiles->m_sFullPath.GetStr());
							allProjectFiles->m_bOpenForEdit = true;
							justloaded = true;
							strcpy(cSearch, cnSearch);
							strcpy(cSearchReplace, cnSearchReplace);
						}

						if (allProjectFiles->m_bOpenForEdit && allProjectFiles->m_editor && !allProjectFiles->m_scene) {
							//Replace all.
							Coordinates oldpos = allProjectFiles->m_editor->mState.mCursorPosition;
							Coordinates oldmSelectionEnd = allProjectFiles->m_editor->mState.mSelectionEnd;
							Coordinates oldmmSelectionStart = allProjectFiles->m_editor->mState.mSelectionStart;

							allProjectFiles->m_editor->mState.mCursorPosition.mLine = 0;
							allProjectFiles->m_editor->mState.mCursorPosition.mColumn = 0;
							allProjectFiles->m_editor->bFindNextEndOfLine = false;
							allProjectFiles->m_editor->mState.mSelectionEnd = Coordinates(0,0);
							allProjectFiles->m_editor->mState.mSelectionStart = Coordinates(0, 0);

							strcpy(allProjectFiles->m_editor->cnSearch, cSearch);

							while (!allProjectFiles->m_editor->bFindNextEndOfLine) {
								allProjectFiles->m_editor->FindNext();
								if (allProjectFiles->m_editor->HasSelection()) {
									justloaded = false; //keep it open we changed the file.
									if (strlen(cnSearchReplace) > 0)
										allProjectFiles->m_editor->PasteString(cnSearchReplace);
									else
										allProjectFiles->m_editor->Cut();

									//Cut , PasteString set cursor after insert so must go one back.
									Coordinates pos = allProjectFiles->m_editor->GetActualCursorCoordinates();
									if (pos.mColumn > 0) pos.mColumn--;
									allProjectFiles->m_editor->SetCursorPosition(pos);

									replaced++;
								}
							}
							allProjectFiles->m_editor->mState.mCursorPosition = oldpos;
							allProjectFiles->m_editor->mState.mSelectionEnd = oldmSelectionEnd;
							allProjectFiles->m_editor->mState.mSelectionStart = oldmmSelectionStart;

							allProjectFiles->m_editor->bFindNextEndOfLine = false;
						}

						if(justloaded && !allProjectFiles->m_editor->filechanged ) {
							//Just close it again. nothing was changed.
							allProjectFiles->m_editor->quiteditor = true;
						}

						allProjectFiles = allProjectFiles->m_pNext;
					}
					sprintf(ReplaceMsg, "Found and replaced %d times.", replaced);

				}
			}
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 6, ImGui::GetCursorPos().y));
			if (ImGui::Button("In Selection")) {
				
				strcpy(ReplaceMsg, "");
				bUndoAll = 0;
				Coordinates oldpos = mState.mCursorPosition;
				//Start replace all at the top.
				mState.mCursorPosition.mLine = 0;
				mState.mCursorPosition.mColumn = 0;
				bFindPrevStartOfLine = false;

				auto oldSelEnd = mState.mSelectionEnd;
				auto oldSelStart = mState.mSelectionStart;

				mState.mCursorPosition = mState.mSelectionEnd;

				int replaced = 0;
				while (!bFindPrevStartOfLine) {
					FindPrev();

					if (!bFindPrevStartOfLine && HasSelection()) {
						if (mState.mSelectionStart >= oldSelStart && mState.mSelectionEnd <= oldSelEnd) {
							auto oldstart = mState.mSelectionStart;
							auto oldend = mState.mSelectionEnd;
							auto oldcursor = mState.mCursorPosition;
							if (strlen(cSearchReplace) > 0) {
								PasteString(cSearchReplace);
							}
							else {
								Cut();
							}
							mState.mSelectionStart = oldstart;
							mState.mSelectionEnd = oldend;
//							mState.mCursorPosition = oldcursor;
							//if (oldstart.mColumn > 0)
							oldstart.mColumn++;
							mState.mCursorPosition = oldstart;

							replaced++;
						}
						else
						{
							if (mState.mSelectionStart < oldSelStart)
							{
								//Below selection start. end.
								bFindPrevStartOfLine = true; //Break out.
							}
						}
					}
				}
				bool bt = m_ActiveEditor->bFindNextEndOfLine;
				//m_ActiveEditor->cSearch;
				mState.mSelectionEnd = oldSelEnd;
				mState.mSelectionStart = oldSelStart;

				sprintf(ReplaceMsg, "Found and replaced %d times.", replaced);
				bUndoAll = replaced;
				bFindNextEndOfLine = false;
				bFindPrevStartOfLine = false;
				mState.mCursorPosition = oldpos;
			}


		}


		if (bFindNextEndOfLine) {
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 12, ImGui::GetCursorPos().y + 2 ) );
			ImGui::TextColored(ImVec4(1.0, 0.3, 0.3, 1.0), "None found: reached end of file.");
		}
		else if (bFindPrevStartOfLine) {
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 12, ImGui::GetCursorPos().y + 2));
			ImGui::TextColored(ImVec4(1.0, 0.3, 0.3, 1.0), "None found: reached start of file.");
		}
		else if (strlen(ReplaceMsg) > 0) {
			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 12, ImGui::GetCursorPos().y + 2));
			ImGui::TextColored(ImVec4(1.0, 0.3, 0.3, 1.0), "%s",ReplaceMsg );
			if (bUndoAll > 0) {
				ImGui::SameLine();
				if (ImGui::Button("Undo All")) {
					for (int ml = 0; ml < bUndoAll; ml++)
						Undo();
					bUndoAll = 0;
				}
			}
		}
		
		ImGui::EndChild();
		ImGui::SetCursorPos(oldCursor);

		if (!bSearchActive) {
			ImGui::SetWindowFocus();
			ImGui::SetKeyboardFocusHere();
		}
	}


	// Deduce mTextStart by evaluating mLines size (global lineMax) plus two spaces as text width
	char buf[20];
	if (display_linenumbers) {
		snprintf(buf, 16, " %d ", globalLineMax);
	}
	else {
		strcpy(buf, " ");
	}
	if (pref.iEnableCodeFolding)
		strcat(buf, " ");

	mTextStart = ImGui::CalcTextSize(buf).x + mLeftMargin;

	//0xff181818
	//PaletteIndex::Background
//	unsigned int bc = mPalette[(int)PaletteIndex::Background];
//	bc &= 0xfff4f4f4;
	//BackgroundLine
	ImVec2 drawpos = ImGui::GetWindowPos();
	drawpos.x -= ImGui::GetScrollX();

	ImGui::GetWindowDrawList()->AddRectFilled(drawpos, drawpos + ImVec2(mTextStart-4.0,ImGui::GetWindowSize().y), mPalette[(int)PaletteIndex::BackgroundLine] );


	if (ImGui::IsWindowHovered()) {
		ImVec2 origin = ImGui::GetCursorScreenPos();
		float mx = ImGui::GetMousePos().x - origin.x;
		if ( mx < (mTextStart - 4.0 )) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
			if (ImGui::IsMouseClicked(0)) {
				if (mx > TOGGLEBREAKPOINTX) {
					bEnableSuggest = false;
					bSuggestActice = false;
					mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = SanitizeCoordinates(ScreenPosToCoordinates(ImGui::GetMousePos(), true));
					mSelectionMode = SelectionMode::Line;
					SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);
				}
			}
		}
	}

	if (!mLines.empty())
	{
		auto fontScale = ImGui::GetFontSize() / ImGui::GetFont()->FontSize;
		float spaceSize = ImGui::CalcTextSize(" ").x + 1.0f * fontScale;

		bInFolding = false;
		bFoldNow = false;
		breakonnext = false;
		iFoldeStarters = 0;

		int iCountlineNo = lineNo; 
		int iScreenLine = scLineNo;
		
		//iSpeedColorize = 1.0 - iSpeedColorize;
		iSpeedColorize = 0;

		while (iCountlineNo <= lineMax)
		{
			//PE: iCountlineNo need to decrease by hidden lines not in iFoldingLines.

			lineNo = iCountlineNo + iFoldingLines;

			if (lineNo < mLines.size()) {
				auto& line = mLines[lineNo];

				//PE: enable code folding.
				if (pref.iEnableCodeFolding && lineNo < MAXPRECHECKLINES) { //Disable code folding here.

					//PE: This is flashing, we need to take topscreen and botscreen instead.
//					if (iSpeedColorize) {
//						if( lineNo & 1)
//							ColorizeRange(lineNo, lineNo + 1);
//					}
//					else {
//						if (!(lineNo & 1))
//							ColorizeRange(lineNo, lineNo + 1);
//					}
					if(iSpeedColorize++ < 80)
						ColorizeRange(lineNo, lineNo + 1);


//					char * buffer = new char[line.size() + 1];
//					for (size_t j = 0; j < line.size(); ++j)
//					{
//						auto& col = line[j];
//						buffer[j] = col.mChar;
//						buffer[j + 1] = 0;
//					}

/*

					if (bInFolding || iFoldeStarters > 0) {
						//already in folding, find endword.

						//m_bFoldingPossible[lineNo] = true;

						if (m_bFoldingEndWord[lineNo]) {
							if (--iFoldeStarters <= 0)
								bInFolding = false;
						}

						if (bFoldNow) {
							//Dont display line.
							if (m_bFoldingStartWord[lineNo]) {
								if (iFoldeStarters < 0) iFoldeStarters = 0;
									iFoldeStarters++;
							}
							iFoldingLines++;

							//Calc new lineMax.
							lineMax = std::max(0, std::min((int)mLines.size() - 1, ((int)floor(scrollY / mCharAdvance.y) + iFoldingLines) + (int)floor((scrollY + contentSize.y) / mCharAdvance.y)));

							m_bLineVisible[lineNo] = false;

							//delete[] buffer;
							if ( (m_bFoldingEndWord[lineNo] || m_bFoldingStartWord[lineNo]) && (iFoldeStarters <= 0 || breakonnext)) {
								bFoldNow = false;
								breakonnext = false;
								m_bLineVisible[lineNo] = true;
							}
							continue;
						}
						else {
							m_bLineVisible[lineNo] = true;
						}

					}
					else {
						//if (!m_bLineVisible[lineNo]) {
							//m_bFoldingPossible[lineNo] = false;
						//}
						//m_bLineVisible[lineNo] = true;
					}

*/

					if (!m_bLineVisible[lineNo]) {
						iFoldingLines++;
						//Calc new lineMax.
						//lineMax = std::max(0, std::min((int)mLines.size() - 1, ((int)floor(scrollY / mCharAdvance.y) + iFoldingLines) + (int)floor((scrollY + contentSize.y) / mCharAdvance.y)));
						lineMax++;
						//delete[] buffer;
						continue;
					}

					if (m_bFoldingStartWord[lineNo])
					{
						//Add button.
						ImVec2 oldcursorpos = ImGui::GetCursorPos();
						auto lineNoWidth = ImGui::CalcTextSize("#").x;

						if (iFoldeStarters < 0) iFoldeStarters = 0;
						iFoldeStarters++;
						float butstartx = (mTextStart - (mCharAdvance.x*2.5f));
						if (butstartx < (mLeftMargin*0.75)) butstartx = (mLeftMargin*0.75);
						ImGui::SetCursorPos(ImVec2(butstartx, (iScreenLine*mCharAdvance.y)));

						uString uniqueStr;

						//if (m_bFoldingActive[lineNo]) {

						if (mFoldingActive.count(lineNo + 1) != 0)
						{
							//+
							uniqueStr = "+##uscf";
							uniqueStr.AppendInt(lineNo);
							ImGui::SetWindowFontScale(0.85);
							if (ImGui::SmallButton(uniqueStr)) {

								mFoldingActive.erase(lineNo + 1);
								mState.mSelectionEnd = mState.mSelectionStart = mState.mCursorPosition;
								//m_bFoldingActive[lineNo] = false;
							}
							ImGui::SetWindowFontScale(1.0);
							if (iFoldeStarters > 1)
								breakonnext = true;
							bFoldNow = true;
						}
						else {
							//-
							uniqueStr = "-##uscf";
							uniqueStr.AppendInt(lineNo);
							//ImGui::
							ImGui::SetWindowFontScale(0.85);
							if (ImGui::SmallButton(uniqueStr)) {

								mFoldingActive.insert(lineNo + 1);
								mState.mSelectionEnd = mState.mSelectionStart = mState.mCursorPosition;

								//m_bFoldingActive[lineNo] = true;
								//iFoldeStarters = 1; //Stop at next entry.
							}
							ImGui::SetWindowFontScale(1.0);
							bFoldNow = false;
						}
						//m_bFoldingActive[lineNo] = false;
						//m_bFoldingPossible[lineNo] = true;
						bInFolding = true;
						ImGui::SetCursorPos(oldcursorpos);
					}
					//delete[] buffer;

				}

				
//				ImVec2 lineStartScreenPos = ImVec2(cursorScreenPos.x, cursorScreenPos.y + lineNo * mCharAdvance.y);
//				ImVec2 textScreenPos = ImVec2(lineStartScreenPos.x + mTextStart, lineStartScreenPos.y);

//				ImVec2 lineStartScreenPos = ImVec2(cursorScreenPos.x, cursorScreenPos.y + iCountlineNo * mCharAdvance.y);
//				ImVec2 textScreenPos = ImVec2(lineStartScreenPos.x + mTextStart, lineStartScreenPos.y);

				ImVec2 lineStartScreenPos = ImVec2(cursorScreenPos.x, cursorScreenPos.y + iScreenLine * mCharAdvance.y);
				ImVec2 textScreenPos = ImVec2(lineStartScreenPos.x + mTextStart, lineStartScreenPos.y);

				//lineNo = 

				//PE: moved below.
				//longest = std::max(mTextStart + TextDistanceToLineStart(Coordinates(lineNo, (int) line.size())), longest);
				auto columnNo = 0;
				Coordinates lineStartCoord(lineNo, 0);
				Coordinates lineEndCoord(lineNo, (int)line.size());

				//	Draw Selected area 

				float sstart = -1.0f;
				float ssend = -1.0f;

				assert(mState.mSelectionStart <= mState.mSelectionEnd);
				if (mState.mSelectionStart <= lineEndCoord)
					sstart = mState.mSelectionStart > lineStartCoord ? TextDistanceToLineStart(mState.mSelectionStart) : 0.0f;
				if (mState.mSelectionEnd > lineStartCoord)
					ssend = TextDistanceToLineStart(mState.mSelectionEnd < lineEndCoord ? mState.mSelectionEnd : lineEndCoord);

				if (mState.mSelectionEnd.mLine > lineNo)
					ssend += mCharAdvance.x;

				if (sstart != -1 && ssend != -1 && sstart < ssend)
				{
					ImVec2 vstart(lineStartScreenPos.x + mTextStart + sstart, lineStartScreenPos.y);
					ImVec2 vend(lineStartScreenPos.x + mTextStart + ssend, lineStartScreenPos.y + mCharAdvance.y);
					drawList->AddRectFilled(vstart, vend, mPalette[(int)PaletteIndex::Selection]);
				}

				//Highlight any partner brackets.
				if (mState.mCursorPosition.mLine == lineNo)
				{
					if (mState.mCursorPosition.mColumn < line.size()  && mState.mCursorPosition.mColumn - 1 > 0 ) {
						char bracket = (unsigned char) line[mState.mCursorPosition.mColumn].mChar; //
						int bracket_entrys = 0;

						if (bracket == ')') {
							for (int i = mState.mCursorPosition.mColumn-1; i > 0; i--) {
								if (line[i].mChar == ')') bracket_entrys++;
								if (line[i].mChar == '(' && bracket_entrys == 0) {
									//Highligt the positions.
									sstart = TextDistanceToLineStart(Coordinates(lineNo, i));
									ssend = TextDistanceToLineStart(Coordinates(lineNo, i+1));
									ImVec2 vstart(lineStartScreenPos.x + mTextStart + sstart, lineStartScreenPos.y);
									ImVec2 vend(lineStartScreenPos.x + mTextStart + ssend, lineStartScreenPos.y + mCharAdvance.y);
									drawList->AddRectFilled(vstart, vend, mPalette[(int)PaletteIndex::CurrentLineFill]);

									sstart = TextDistanceToLineStart(Coordinates(lineNo, mState.mCursorPosition.mColumn));
									ssend = TextDistanceToLineStart(Coordinates(lineNo, mState.mCursorPosition.mColumn + 1));
									vstart = ImVec2(lineStartScreenPos.x + mTextStart + sstart, lineStartScreenPos.y);
									vend = ImVec2(lineStartScreenPos.x + mTextStart + ssend, lineStartScreenPos.y + mCharAdvance.y);
									drawList->AddRectFilled(vstart, vend, mPalette[(int)PaletteIndex::CurrentLineFill]);
									break;
								}
								if (line[i].mChar == '(' && bracket_entrys > 0) bracket_entrys--;
							}
						}
						else if (bracket == '(')
						{
							for (int i = mState.mCursorPosition.mColumn + 1; i < line.size(); i++) {
								if (line[i].mChar == '(') bracket_entrys++;
								if (line[i].mChar == ')' && bracket_entrys == 0) {
									//Highligt the positions.
									sstart = TextDistanceToLineStart(Coordinates(lineNo, i));
									ssend = TextDistanceToLineStart(Coordinates(lineNo, i + 1));
									ImVec2 vstart(lineStartScreenPos.x + mTextStart + sstart, lineStartScreenPos.y);
									ImVec2 vend(lineStartScreenPos.x + mTextStart + ssend, lineStartScreenPos.y + mCharAdvance.y);
									drawList->AddRectFilled(vstart, vend, mPalette[(int)PaletteIndex::CurrentLineFill]);

									sstart = TextDistanceToLineStart(Coordinates(lineNo, mState.mCursorPosition.mColumn));
									ssend = TextDistanceToLineStart(Coordinates(lineNo, mState.mCursorPosition.mColumn + 1));
									vstart = ImVec2(lineStartScreenPos.x + mTextStart + sstart, lineStartScreenPos.y);
									vend = ImVec2(lineStartScreenPos.x + mTextStart + ssend, lineStartScreenPos.y + mCharAdvance.y);
									drawList->AddRectFilled(vstart, vend, mPalette[(int)PaletteIndex::CurrentLineFill]);
									break;
								}
								if (line[i].mChar == ')' && bracket_entrys > 0) bracket_entrys--;
							}
						}


						if (bracket == ']') {
							for (int i = mState.mCursorPosition.mColumn - 1; i > 0; i--) {
								if (line[i].mChar == ']') bracket_entrys++;
								if (line[i].mChar == '[' && bracket_entrys == 0) {
									//Highligt the positions.
									sstart = TextDistanceToLineStart(Coordinates(lineNo, i));
									ssend = TextDistanceToLineStart(Coordinates(lineNo, i + 1));
									ImVec2 vstart(lineStartScreenPos.x + mTextStart + sstart, lineStartScreenPos.y);
									ImVec2 vend(lineStartScreenPos.x + mTextStart + ssend, lineStartScreenPos.y + mCharAdvance.y);
									drawList->AddRectFilled(vstart, vend, mPalette[(int)PaletteIndex::CurrentLineFill]);

									sstart = TextDistanceToLineStart(Coordinates(lineNo, mState.mCursorPosition.mColumn));
									ssend = TextDistanceToLineStart(Coordinates(lineNo, mState.mCursorPosition.mColumn + 1));
									vstart = ImVec2(lineStartScreenPos.x + mTextStart + sstart, lineStartScreenPos.y);
									vend = ImVec2(lineStartScreenPos.x + mTextStart + ssend, lineStartScreenPos.y + mCharAdvance.y);
									drawList->AddRectFilled(vstart, vend, mPalette[(int)PaletteIndex::CurrentLineFill]);
									break;
								}
								if (line[i].mChar == '[' && bracket_entrys > 0) bracket_entrys--;
							}
						}
						else if (bracket == '[')
						{
							for (int i = mState.mCursorPosition.mColumn + 1; i < line.size(); i++) {
								if (line[i].mChar == '[') bracket_entrys++;
								if (line[i].mChar == ']' && bracket_entrys == 0) {
									//Highligt the positions.
									sstart = TextDistanceToLineStart(Coordinates(lineNo, i));
									ssend = TextDistanceToLineStart(Coordinates(lineNo, i + 1));
									ImVec2 vstart(lineStartScreenPos.x + mTextStart + sstart, lineStartScreenPos.y);
									ImVec2 vend(lineStartScreenPos.x + mTextStart + ssend, lineStartScreenPos.y + mCharAdvance.y);
									drawList->AddRectFilled(vstart, vend, mPalette[(int)PaletteIndex::CurrentLineFill]);

									sstart = TextDistanceToLineStart(Coordinates(lineNo, mState.mCursorPosition.mColumn));
									ssend = TextDistanceToLineStart(Coordinates(lineNo, mState.mCursorPosition.mColumn + 1));
									vstart = ImVec2(lineStartScreenPos.x + mTextStart + sstart, lineStartScreenPos.y);
									vend = ImVec2(lineStartScreenPos.x + mTextStart + ssend, lineStartScreenPos.y + mCharAdvance.y);
									drawList->AddRectFilled(vstart, vend, mPalette[(int)PaletteIndex::CurrentLineFill]);
									break;
								}
								if (line[i].mChar == ']' && bracket_entrys > 0) bracket_entrys--;
							}
						}


					}
				}

				//More selected based on keyword below cursor.
				//lastKeywordSelected
				if (strlen(lastKeywordSelected) > 1 && line.size() >= strlen(lastKeywordSelected)) {
					char * buffer = new char[line.size() + 1];
					for (size_t j = 0; j < line.size(); ++j)
					{
						auto& col = line[j];
						buffer[j] = col.mChar;
						buffer[j + 1] = 0;
					}

					char *found = (char*)pestrcasestr(&buffer[0], lastKeywordSelected);
					while (found)
					{
						//
						int pos = found - &buffer[0];
						if (pos >= 0 && pos < line.size()) {
							if (mState.mSelectionStart.mLine == lineNo && mState.mSelectionStart.mColumn == pos) {
								//Already marked.
							}
							else {

								sstart = TextDistanceToLineStart(Coordinates(lineNo, pos));
								Coordinates mend = Coordinates(lineNo, pos + strlen(lastKeywordSelected));
								ssend = TextDistanceToLineStart(mend < lineEndCoord ? mend : lineEndCoord);
								ImVec2 vstart(lineStartScreenPos.x + mTextStart + sstart, lineStartScreenPos.y);
								ImVec2 vend(lineStartScreenPos.x + mTextStart + ssend, lineStartScreenPos.y + mCharAdvance.y);
								drawList->AddRectFilled(vstart, vend, mPalette[(int)PaletteIndex::CurrentLineFill]);
							}
						}
						if (*found + 1 == 0)
							break;
						found = (char*)pestrcasestr(found + 1, lastKeywordSelected);
					}
					delete[] buffer;
				}

				//	Draw break point

				auto start = ImVec2(lineStartScreenPos.x + scrollX, lineStartScreenPos.y);

				//if (bpts.count(lineNo + 1) != 0)
				if (mBreakpoints.count(lineNo + 1) != 0)
				{
					auto end = ImVec2(lineStartScreenPos.x + contentSize.x + 2.0f * scrollX, lineStartScreenPos.y + mCharAdvance.y);
					//				drawList->AddRectFilled(start, end, mPalette[(int)PaletteIndex::Breakpoint]);
					ImVec2 margin;
					margin.x = mCharAdvance.y*0.5;
					margin.y = mCharAdvance.y*0.5;
					float radius = mCharAdvance.y * 0.375;
					//If scaled font , make sure it dont get to large.
					if (radius + margin.x > (mLeftMargin*0.75)) {
						margin.x = 8.5;
						radius = 6.375;
					}
					drawList->AddCircleFilled(start + margin, radius, mPalette[(int)PaletteIndex::ErrorMarker]);
					//AddCircle.
				}

				if (executeLine == lineNo + 1) {
					auto end = ImVec2(lineStartScreenPos.x + contentSize.x + 2.0f * scrollX, lineStartScreenPos.y + mCharAdvance.y);
					drawList->AddRectFilled(start, end, mPalette[(int)PaletteIndex::Breakpoint]);
				}

				//	Draw error marker

				if (!modalopen) {
					auto errorIt = mErrorMarkers.find(lineNo + 1);
					if (errorIt != mErrorMarkers.end())
					{
						auto end = ImVec2(lineStartScreenPos.x + contentSize.x + 2.0f * scrollX, lineStartScreenPos.y + mCharAdvance.y);
						auto draw = ImVec2(lineStartScreenPos.x + mTextStart - 2.0f * scrollX, lineStartScreenPos.y + mCharAdvance.y);

						drawList->AddRectFilled(start, draw, mPalette[(int)PaletteIndex::ErrorMarker]);

						if (ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(lineStartScreenPos, end))
						{
							//Set cursorpos a bit down.
							//tooltip always follow mouse.
							ImVec2 cursor_pos = ImGui::GetIO().MousePos;
							ImVec2 tooltip_offset(10.0f, mCharAdvance.y);
							ImVec2 tooltip_position = cursor_pos;
							tooltip_position.x += tooltip_offset.x;
							tooltip_position.y += tooltip_offset.y;
							ImGui::SetNextWindowPos(tooltip_position);


							ImGui::BeginTooltip();
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
							ImGui::Text("Error at line %d:", errorIt->first);
							ImGui::PopStyleColor();
							ImGui::Separator();
							//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f));
							ImGui::Text("%s", errorIt->second.c_str());
							//ImGui::PopStyleColor();
							ImGui::EndTooltip();
						}
					}
				}

				//	Draw line number (right aligned)
				if (display_linenumbers) {
					if (pref.iEnableCodeFolding) {

						if (lineNo < MAXPRECHECKLINES && m_bFoldingPossible[lineNo]) {
							int mylinex = ImGui::CalcTextSize("-").x*1.65; //mCharAdvance.x;
							if (m_bFoldingEndWord[lineNo]) {
								if (lineNo < mLines.size() - 1 && (!m_bFoldingPossible[lineNo+1] && !m_bFoldingStartWord[lineNo + 1]) ) {
									drawList->AddLine(ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y), ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y + (mCharAdvance.y*0.5)), mPalette[(int)PaletteIndex::LineNumber], 1.0f);
									drawList->AddLine(ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y + (mCharAdvance.y*0.5)), ImVec2(lineStartScreenPos.x + mTextStart - (mylinex*0.5), lineStartScreenPos.y + (mCharAdvance.y*0.5)), mPalette[(int)PaletteIndex::LineNumber], 1.0f);
								}
								else {
									drawList->AddLine(ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y), ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y + mCharAdvance.y), mPalette[(int)PaletteIndex::LineNumber], 1.0f);
									drawList->AddLine(ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y + (mCharAdvance.y*0.5)), ImVec2(lineStartScreenPos.x + mTextStart - (mylinex*0.5), lineStartScreenPos.y + (mCharAdvance.y*0.5)), mPalette[(int)PaletteIndex::LineNumber], 1.0f);
								}
							}
							else
								drawList->AddLine(ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y), ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y + mCharAdvance.y), mPalette[(int)PaletteIndex::LineNumber], 1.0f);

//							snprintf(buf, 17, "%d | ", lineNo + 1);
							snprintf(buf, 17, "%d   ", lineNo + 1);
						}
						else {
							snprintf(buf, 17, "%d   ", lineNo + 1);
						}
					}
					else
					{
						snprintf(buf, 16, "%d  ", lineNo + 1);
					}

					auto lineNoWidth = ImGui::CalcTextSize(buf).x;

					drawList->AddText(ImVec2(lineStartScreenPos.x + mTextStart - lineNoWidth, lineStartScreenPos.y), mPalette[(int)PaletteIndex::LineNumber], buf);
				}
				else {

					if (pref.iEnableCodeFolding) {

						if (lineNo < MAXPRECHECKLINES && m_bFoldingPossible[lineNo]) {
							int mylinex = ImGui::CalcTextSize("-").x*1.65; //mCharAdvance.x;
							if (m_bFoldingEndWord[lineNo]) {
								if (lineNo < mLines.size() - 1 && (!m_bFoldingPossible[lineNo + 1] && !m_bFoldingStartWord[lineNo + 1])) {
									drawList->AddLine(ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y), ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y + (mCharAdvance.y*0.5)), mPalette[(int)PaletteIndex::LineNumber], 1.0f);
									drawList->AddLine(ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y + (mCharAdvance.y*0.5)), ImVec2(lineStartScreenPos.x + mTextStart - (mylinex*0.5), lineStartScreenPos.y + (mCharAdvance.y*0.5)), mPalette[(int)PaletteIndex::LineNumber], 1.0f);
								}
								else {
									drawList->AddLine(ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y), ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y + mCharAdvance.y), mPalette[(int)PaletteIndex::LineNumber], 1.0f);
									drawList->AddLine(ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y + (mCharAdvance.y*0.5)), ImVec2(lineStartScreenPos.x + mTextStart - (mylinex*0.5), lineStartScreenPos.y + (mCharAdvance.y*0.5)), mPalette[(int)PaletteIndex::LineNumber], 1.0f);
								}
							}
							else
								drawList->AddLine(ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y), ImVec2(lineStartScreenPos.x + mTextStart - mylinex, lineStartScreenPos.y + mCharAdvance.y), mPalette[(int)PaletteIndex::LineNumber], 1.0f);
						}
					}
				}

				if (mBookMarks.count(lineNo + 1) != 0)
				{
					auto end = ImVec2(lineStartScreenPos.x + contentSize.x + 2.0f * scrollX, lineStartScreenPos.y + mCharAdvance.y);
					ImVec2 margin;
					margin.x = mCharAdvance.y*0.5;
					margin.y = mCharAdvance.y*0.5;
					float radius = mCharAdvance.y * 0.375;
					//If scaled font , make sure it dont get to large.
					if (radius + margin.x > (mLeftMargin*0.75)) {
						margin.x = 8.5;
						radius = 6.375;
					}
					//A bit largen then breakpoint.
					radius += 0.5;
					drawList->AddCircleFilled(start + margin, radius, mPalette[(int)PaletteIndex::Selection] );
/* old code , now placed at same spot as breakpoint so dont go into code folding.
					auto end = ImVec2(lineStartScreenPos.x + contentSize.x + 2.0f * scrollX, lineStartScreenPos.y + mCharAdvance.y);
					ImVec2 margin;
					margin.x = mCharAdvance.y*0.5;
					margin.y = mCharAdvance.y*0.5;
					float radius = mCharAdvance.y * 0.375;
					float xcenter = (margin.x - radius) * 0.5;
					//				drawList->AddCircleFilled(start + margin + ImVec2(4.0, 0.0), radius, mPalette[(int)PaletteIndex::Selection]);
					drawList->AddCircleFilled(ImVec2(lineStartScreenPos.x + mTextStart - margin.x - xcenter, lineStartScreenPos.y + margin.y), radius, mPalette[(int)PaletteIndex::Selection]);
*/
				}


				//Display mInteractiveStart , debug only.
//				if (mInteractiveStart.mLine == lineNo)
//				{
//					float cx = TextDistanceToLineStart(mInteractiveStart);
//					ImVec2 cstart(textScreenPos.x + cx, lineStartScreenPos.y);
//					ImVec2 cend(textScreenPos.x + cx + 2.0f , lineStartScreenPos.y + mCharAdvance.y);
//					drawList->AddRectFilled(cstart, cend, mPalette[(int)PaletteIndex::ErrorMarker]);
//
//				}
				

				//	Highlight the current line (where the cursor is).
				if (mState.mCursorPosition.mLine == lineNo)
				{
					auto focused = ImGui::IsWindowFocused();

					//				if (!HasSelection())
					//				{
					//					auto end = ImVec2(start.x + contentSize.x + scrollX, start.y + mCharAdvance.y);
					//					//drawList->AddRectFilled(start, end, mPalette[(int)(focused ? PaletteIndex::CurrentLineFill : PaletteIndex::CurrentLineFillInactive)]);
					//					drawList->AddRect(start, end, mPalette[(int)PaletteIndex::CurrentLineEdge], 1.0f);
					//				}

					float cx = TextDistanceToLineStart(mState.mCursorPosition);

					if (focused)
					{
						static bool blinkstate = false;
						auto timeEnd = std::chrono::system_clock::now();
						auto diff = timeEnd - timeStart;
						auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
						if (elapsed >= 0) //PE: Make sure cursor is drawed when key is pressed.
						{
							extern bool ide_force_rendering;
							if (elapsed <= 400) {
								ImVec2 cstart(textScreenPos.x + cx, lineStartScreenPos.y);
								ImVec2 cend(textScreenPos.x + cx + (mOverwrite ? mCharAdvance.x : 1.0f), lineStartScreenPos.y + mCharAdvance.y);
								drawList->AddRectFilled(cstart, cend, mPalette[(int)PaletteIndex::Cursor]);
								if (!blinkstate) {
									ide_force_rendering = true;
									blinkstate = true;
								}
							}
							else {
								if (blinkstate) {
									ide_force_rendering = true;
									blinkstate = false;
								}
							}
							if (elapsed > 800) {
								//Always recolor current line every 800 ms ?
								if (mState.mCursorPosition.mLine < MAXPRECHECKLINES)
									m_bFirstCheck[mState.mCursorPosition.mLine] = false;
								FastColorizeRange(mState.mCursorPosition.mLine, mState.mCursorPosition.mLine + 1);

								timeStart = timeEnd;
							}
						}

					}

				}

				//	Draw Text

				auto prevColor = line.empty() ? PaletteIndex::Default : (line[0].mMultiLineComment ? PaletteIndex::MultiLineComment : line[0].mColorIndex);
				ImVec2 bufferOffset;

				for (auto& glyph : line)
				{
					auto color = glyph.mMultiLineComment ? PaletteIndex::MultiLineComment : glyph.mColorIndex;

					if ((color != prevColor || glyph.mChar == '\t') && !buffer.empty())
					{
						const ImVec2 newOffset(textScreenPos.x + bufferOffset.x, textScreenPos.y + bufferOffset.y);
						drawList->AddText(newOffset, mPalette[(uint8_t)prevColor], buffer.c_str());
						auto textSize = ImGui::CalcTextSize(buffer.c_str());
						bufferOffset.x += textSize.x + 1.0f * fontScale;
						buffer.clear();
					}
					prevColor = color;

					if (glyph.mChar == '\t') {
						float oldx = bufferOffset.x;
						//* fontScale
						bufferOffset.x = (1.0f + std::floor((1.0f + bufferOffset.x)) / (float(mTabSize) * spaceSize)) * (float(mTabSize) * spaceSize);
						//bufferOffset.x = (1.0f * fontScale + std::floor((1.0f + bufferOffset.x)) / (float(mTabSize) * spaceSize)) * (float(mTabSize) * spaceSize);
						bufferOffset.x = roundByTabSize(bufferOffset.x, mTabSize, spaceSize, oldx);
					}
					else {
						//PE: Render using UTF8, all text is stored in unicode.
						char utf8[4];

						//UTF8: need support here.
						unsigned int c = (unsigned char)glyph.mChar;
						if (glyph.mInt > 0) {
							std::string ms = UnicodeToUTF8(glyph.mInt);
							for (int a = 0; a < ms.size(); a++) {
								AppendBuffer(buffer, ms[a], 0);
							}
						}
						else {
							if (CharToUtf8(&utf8[0], 2, c) == 2)
							{
								AppendBuffer(buffer, utf8[0], 0);
								AppendBuffer(buffer, utf8[1], 0);
							}
							else {
								AppendBuffer(buffer, glyph.mChar, 0);
							}
						}
					}
					++columnNo;
				}

				if (!buffer.empty())
				{
					const ImVec2 newOffset(textScreenPos.x + bufferOffset.x, textScreenPos.y + bufferOffset.y);
					drawList->AddText(newOffset, mPalette[(uint8_t)prevColor], buffer.c_str());

					auto textSize = ImGui::CalcTextSize(buffer.c_str());
					bufferOffset.x += textSize.x + 1.0f * fontScale;

					buffer.clear();
				}

				//Reuse bufferOffset for logest
				longest = std::max(mTextStart + bufferOffset.x, longest);

				//++lineNo;
			}
			++iCountlineNo;
			++iScreenLine;
		}

		if (ImGui::IsMousePosValid())
		{
			//debug_is_running
			if( ImGui::IsWindowHovered() && (remote_debug_is_running || debug_is_running) && isAGKsource && ImGui::IsWindowFocused() && wtabvisible ) {
				//
				Coordinates mpos = ScreenPosToCoordinates(ImGui::GetMousePos(), true);
				auto id = GetWordAt(mpos,true);
				if (!id.empty())
				{
					if (id.length() < MAX_PATH) {

						if (id.find('"') == std::string::npos &&
							id.find('=') == std::string::npos &&
							id.find('(') == std::string::npos &&
							id.find(')') == std::string::npos &&
							id.find('/') == std::string::npos &&
							id.find('+') == std::string::npos ) {

							uString szTmp;
							szTmp = id.c_str();
							szTmp.ReplaceStr(" ", "");
							//must not be a keyword.
							if (mLanguageDefinition.mKeywords.count(szTmp.GetStr()) <= 0) {

								//must start with a alpha.
								if ((szTmp[0] >= 'a' && szTmp[0] <= 'z') || (szTmp[0] >= 'A' && szTmp[0] <= 'Z')) {

									if (atoi(szTmp.GetStr()) <= 0) { // filter out numbers only.

										if (strcmp(szTmp.GetStr(), cHoverGetVariable) != 0) {
											strcpy(cHoverGetVariable, szTmp.GetStr());
											fHoverTime = agk::Timer();
										}

										//Delay hover a bit so it can update.
										if (agk::Timer() - fHoverTime >= 0.2) {
											ImVec2 cursor_pos = ImGui::GetIO().MousePos;
											ImVec2 tooltip_offset(10.0f, mCharAdvance.y);
											ImVec2 tooltip_position = cursor_pos;
											tooltip_position.x += tooltip_offset.x;
											tooltip_position.y += tooltip_offset.y;
											ImGui::SetNextWindowPos(tooltip_position);
											if (strlen(cHoverValue) > 100) {
												ImGui::SetNextWindowContentSize(ImVec2(500, 0));
												ImGui::BeginTooltip();
												ImGui::TextWrapped("%s : %s", cHoverVariable, cHoverValue);
												ImGui::EndTooltip();
											}
											else {
												ImGui::BeginTooltip();
												ImGui::Text("%s : %s", cHoverVariable, cHoverValue);
												ImGui::EndTooltip();
											}
										}
									}
								}
							}
						}
					}
//					agk::Print(id.c_str());
				}
			}

			//Syntax help , when mouse hover over a word. (Tooltip).

			if (!bContextActive && pref.iHoverSyntaxHelp >= 1 && ImGui::IsWindowHovered() && isAGKsource && ImGui::IsWindowFocused() && wtabvisible ) {

				float mx = ImGui::GetMousePos().x;
				float wx = ImGui::GetWindowPos().x;

				float wy = ImGui::GetWindowPos().y;
				float wsy = wy + ImGui::GetWindowSize().y - 20.0;

				//mx > mTextStart &&

				ImVec2 origin = ImGui::GetCursorScreenPos();

				if ( (mx - origin.x) > mTextStart && mx > wx && ImGui::GetMousePos().y > wy && ImGui::GetMousePos().y < wsy) {

					mousehoverstart = ScreenPosToCoordinates(ImGui::GetMousePos(), false);

					if (mousehoverstart != mousehoverend) {
						mousehovertime = agk::Timer();
						mousehoverend = mousehoverstart;
						mousehoverstring.clear();
						mousehoversearch = true;
					}
					else {
						//Before we trigger a search , make sure mouse is in the same spot for 0.5 sec.
						if (agk::Timer() - mousehovertime > 0.50) {

							if (mousehoversearch) {
								//Only lookup once, then keep result.
								mousehoverstring = GetWordAt(ScreenPosToCoordinates(ImGui::GetMousePos(), false));
								mousehoversearch = false;
								if (!mousehoverstring.empty()) {

									//Must start with alpha.
									if ((mousehoverstring[0] >= 'a' && mousehoverstring[0] <= 'z') || (mousehoverstring[0] >= 'A' && mousehoverstring[0] <= 'Z')) {

										int index = tolower(char(mousehoverstring[0]));
										sKeyNext = sKeywordHelp[index];
										if (sKeyNext && sKeyNext->m_pNext) {

											std::string tmpword = mousehoverstring;
											std::transform(tmpword.begin(), tmpword.end(), tmpword.begin(), ::tolower);

											sKeyNext = sKeyNext->m_pNext;
											mousehoverstring.clear();
											mousehoverstring = "";
											while (sKeyNext) {

												if (strcmp(sKeyNext->m_cLowerCommand, tmpword.c_str()) == 0) {
													//Only display one syntax for now.
													if (sKeyNext->m_cReturn.GetLength() > 0) {
														mousehoverstring.append(sKeyNext->m_cReturn.GetStr());
														mousehoverstring.append(" = ");
													}
													mousehoverstring.append(sKeyNext->m_cCommand);
													mousehoverstring.append(sKeyNext->m_cSyntax.GetStr());
													mousehoverstring.append("\n");
													//break;
												}
												sKeyNext = sKeyNext->m_pNext;
											}
										}
									}
									else {
										mousehoverstring.clear();
									}
								}

							}
							if (!mousehoverstring.empty()) {

								ImGui::BeginTooltip();
								ImGui::TextUnformatted(mousehoverstring.c_str());
								//ImGui::TextWrapped(mousehoverstring.c_str());
								ImGui::EndTooltip();
							}

						}
					}
				}
			}
			/*
			//Not usefull , perhaps display real syntax when hovering ?.
			auto id = GetWordAt(ScreenPosToCoordinates(ImGui::GetMousePos(), false));
			if (!id.empty())
			{
				auto it = mLanguageDefinition.mIdentifiers.find(id);
				if (it != mLanguageDefinition.mIdentifiers.end())
				{
					ImGui::BeginTooltip();
					ImGui::TextUnformatted(it->second.mDeclaration.c_str());
					ImGui::EndTooltip();
				}
				else
				{
					auto pi = mLanguageDefinition.mPreprocIdentifiers.find(id);
					if (pi != mLanguageDefinition.mPreprocIdentifiers.end())
					{
						ImGui::BeginTooltip();
						ImGui::TextUnformatted(pi->second.mDeclaration.c_str());
						ImGui::EndTooltip();
					}
				}
			}
			*/
			

		}
	}

	if (bMouseToWordActive && wtabvisible && !bFreezeUnderMouse) {
		ImVec2 mouse = ImGui::GetMousePos();
		Coordinates mpos = ScreenPosToCoordinates(mouse, true);
		idUnderMouse = GetWordAt(mpos, true, true);
		bFreezeUnderMouse = true;
	}

	if (!dragdropactive) {
		if (pDragDropFile) {
			ImVec2 oldCursor = ImGui::GetCursorPos();
			ImVec2 ws = ImGui::GetWindowSize() - ImVec2(30,30);
			ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX() , ImGui::GetScrollY() ));
			ImGui::Dummy(ws);
			if (ImGui::BeginDragDropTarget())
			{
				ImGui::BeginTooltip();
				ImGui::Text("You can only drag/drop media that\nare placed inside your project folder.");
				ImGui::EndTooltip();
				ImGui::EndDragDropTarget();
			}

			ImGui::SetCursorPos(oldCursor);
		}
	}

	if( dragdropactive ) {
		//Drop target from icons.
		ImVec2 oldCursor = ImGui::GetCursorPos();
		Coordinates old_mstate_mouse = mState.mCursorPosition;
		//Mark drop line.
		Coordinates dropPosition = SanitizeCoordinates(ScreenPosToCoordinates(ImGui::GetMousePos(),false));
		dropPosition.mColumn = 0;

		ImVec2 dropStartScreenPos = ImVec2(0, 0 + dropPosition.mLine * mCharAdvance.y);
//		ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY() + dropStartScreenPos.y));
		ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX()+10, dropStartScreenPos.y));
		if(dropPosition.mLine == mLines.size()-1)
			ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x-20, ImGui::GetWindowSize().y - dropStartScreenPos.y - mCharAdvance.y - 10.0 ));
		else
			ImGui::Dummy(ImVec2(ImGui::GetWindowSize().x - 20, mCharAdvance.y));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TEXT_DROP_TARGET", 0)) // ImGuiDragDropFlags_AcceptNoDrawDefaultRect
			{
				if (pDragDropText) {
					char ** payload_n = (char **) payload->Data;
					if (*payload_n) {
						mState.mCursorPosition = dropPosition;
						InsertTextDirectly(*payload_n);
						InsertTextDirectly("\n");
					}
					pDragDropText = NULL;
				}
			}
			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_MODEL_DROP_TARGET", 0)) // ImGuiDragDropFlags_AcceptNoDrawDefaultRect
			{
				if (pDragDropFile) {
					IM_ASSERT(payload->DataSize == sizeof(cFolderItem::sFolderFiles *));
					cFolderItem::sFolderFiles * payload_n = (cFolderItem::sFolderFiles *) payload->Data;
					payload_n = payload_n->m_dropptr;
					if (payload_n) {
						uString fullfilename, relativeName, ext, relativeNameNoExt;
						fullfilename = payload_n->m_sPath;
						int pos = fullfilename.RevFindStr("/");
						if (!(fullfilename.GetLength() - 1 == pos)) {
							fullfilename.Append("/");
						}
						fullfilename.Append(payload_n->m_sName);

						pos = pecasepos(fullfilename.GetStr(), "/media/");
						if (pos >= 0)
							fullfilename.SubString(relativeName, pos + 7);
						else
							relativeName = payload_n->m_sName;

						pos = relativeName.RevFindStr(".");
						ext = "na";
						if (pos > 0) {
							relativeName.SubString(ext, pos);
						}
						relativeNameNoExt = relativeName;
						pos = relativeName.RevFindStr(".");
						if (pos > 0) {
							relativeName.SubString(relativeNameNoExt, 0, pos);
						}
						char insert[2048];
						insert[0] = 0;
						//.fpe,.png,.jpg,.dds,.jpeg,.gif,.tif,.wav,.ogg,.mp3,.obj,.x,.fbx,.3ds,.dae,.b3d,.3d,.lwo,.ago

						uString variable;
						variable = payload_n->m_sName;
						variable.ReplaceStr(".", "_");
						variable.ReplaceStr(" ", "_");
						variable.ReplaceStr("-", "");
						variable.ReplaceStr("*", "");
						variable.ReplaceStr("+", "");
						variable.ReplaceStr("/", "");
						variable.ReplaceStr("\\", "");
						variable.ReplaceStr("|", "");
						variable.ReplaceStr("$", "");
						variable.ReplaceStr("#", "");
						variable.ReplaceStr("%", "");
						variable.ReplaceStr("(", "");
						variable.ReplaceStr(")", "");
						variable.ReplaceStr("[", "");
						variable.ReplaceStr("]", "");
						variable.ReplaceStr("?", "");
						variable.ReplaceStr("`", "");
						variable.ReplaceStr(",", "");
						variable.ReplaceStr("_ps", "");
						variable.ReplaceStr("_vs", "");
						variable.ReplaceStr("_png", "");
						variable.ReplaceStr("_jpg", "");
						variable.ReplaceStr("_jpeg", "");
						variable.ReplaceStr("_bmp", "");
						variable.ReplaceStr("_gif", "");
						variable.ReplaceStr("_wav", "");
						variable.ReplaceStr("_ogg", "");
						variable.ReplaceStr("_mp3", "");

						if (pestrcasestr(ext.GetStr(), ".png") ||
							pestrcasestr(ext.GetStr(), ".jpg") ||
							pestrcasestr(ext.GetStr(), ".jpeg") ||
							pestrcasestr(ext.GetStr(), ".bmp") ||
							pestrcasestr(ext.GetStr(), ".gif")) {
							sprintf(insert, "img_%s = LoadImage( \"%s\" )\n", variable.GetStr(), relativeName.GetStr());
						}
						if (pestrcasestr(ext.GetStr(), ".wav")) {
							sprintf(insert, "wav_%s = LoadSound( \"%s\" )\n", variable.GetStr(), relativeName.GetStr());
						}
						if (pestrcasestr(ext.GetStr(), ".fbx") ||
							pestrcasestr(ext.GetStr(), ".obj") ||
							pestrcasestr(ext.GetStr(), ".x") ||
							pestrcasestr(ext.GetStr(), ".3ds") ||
							pestrcasestr(ext.GetStr(), ".dae") ||
							pestrcasestr(ext.GetStr(), ".fbx") ||
							pestrcasestr(ext.GetStr(), ".ago") ||
							pestrcasestr(ext.GetStr(), ".lwo") ) {
							sprintf(insert, "obj_%s = LoadObject( \"%s\" )\n", variable.GetStr(), relativeName.GetStr());
						}
						if (pestrcasestr(ext.GetStr(), ".ogg")) {
							sprintf(insert, "ogg_%s = LoadMusicOGG( \"%s\" )\n", variable.GetStr(), relativeName.GetStr());
						}
						if (pestrcasestr(ext.GetStr(), ".mp3")) {
							sprintf(insert, "mp3_%s = LoadMusic( \"%s\" )\n", variable.GetStr(), relativeName.GetStr());
						}
						if (pestrcasestr(ext.GetStr(), ".ps")) {
							sprintf(insert, "ps_%s = LoadShader( \"%s.vs\", \"%s.ps\" )\n", variable.GetStr(), relativeNameNoExt.GetStr(), relativeNameNoExt.GetStr());
						}
						if (pestrcasestr(ext.GetStr(), ".vs")) {
							sprintf(insert, "vs_%s = LoadShader( \"%s.vs\", \"%s.ps\" )\n", variable.GetStr(), relativeNameNoExt.GetStr(), relativeNameNoExt.GetStr());
						}

						if (strlen(insert) > 0) {
							mState.mCursorPosition = dropPosition;
							//mState.mCursorPosition.mLine++;
							InsertTextDirectly(insert);
						}

					}
					pDragDropFile = NULL;
				}
			}
			ImGui::EndDragDropTarget();
		}

		mState.mCursorPosition = old_mstate_mouse;
		ImGui::SetCursorPos(oldCursor);
	}

	//PE: Set scrollbar MAX X,Y
	//PE: code folding must remove "non visible lines".
	if (pref.iEnableCodeFolding && iHiddenLinesNew > 0 ) {
		ImGui::Dummy(ImVec2((longest + 2), (mLines.size()- iHiddenLinesNew) * mCharAdvance.y));
	}
	else {
		ImGui::Dummy(ImVec2((longest + 2), mLines.size() * mCharAdvance.y));
	}
	if (ImGui::IsWindowFocused() && wtabvisible && bEnableSuggest && pref.bEnableAutoCompletion)
	{
		//Suggestions for words.
		Coordinates	cpos = mState.mCursorPosition;
		ImVec4* style_colors = ImGui::GetStyle().Colors;
		if (cpos.mLine >= mLines.size()) {
			cpos.mLine = mLines.size()-1;
		}
		if (cpos.mColumn > 0) cpos.mColumn--;

		bSuggestActice = false;
		auto& line = mLines[cpos.mLine];
		std::string id;
		if(cpos.mColumn < line.size())
			id = GetWordAt(cpos);

		//PE: Get from cursor and 2 left, to compare for " ,".
		bool bPossibleDisplaySyntax = false;
		if (cpos.mColumn < line.size() && cpos.mColumn > 0) {
			if (cpos.mColumn > 0 && line[cpos.mColumn].mChar == ',')
				bPossibleDisplaySyntax = true;
			if (cpos.mColumn > 1 && line[cpos.mColumn - 1].mChar == ' ' && line[cpos.mColumn].mChar == ',')
				bPossibleDisplaySyntax = true;
		}

		std::string idleft; // for words to the left of the dot.
		if (cpos.mLine < mLines.size() && cpos.mLine > 0) {
			if (line.size() > cpos.mColumn ) {
				if (line[cpos.mColumn].mColorIndex == PaletteIndex::Comment || line[cpos.mColumn].mColorIndex == PaletteIndex::String)
					id.clear();
			}
		}
		bool dontsuggestword = false;
		bool bTypesLookupActive = false;
		bool bSymbolLookupActive = true;
		
		bLookupReadOnly = false;

		if (!id.empty() && ( (id.length() >= 2 && id[0] == ']' && id[1] == '.') || (id.length() >= 3 && id[0] == ' ' && id[1] == ']' && id[2] == '.') )  ) {
			while (cpos.mColumn > 0 && line[cpos.mColumn].mChar != '[')
				cpos.mColumn--;
			if (cpos.mColumn > 0) {
				cpos.mColumn--;
				idleft = GetWordAt(cpos);
				if (!idleft.empty())
					bTypesLookupActive = true;
			}
		}
		else if (!id.empty() && id[0] == '.') {
			//On dot. get word to the left and combine.
			if (cpos.mColumn > 0) cpos.mColumn--;
			idleft = GetWordAt(cpos);
			if( !idleft.empty() )
				bTypesLookupActive = true;
		}
		else if( (bEnableFreezeInNextRun && !id.empty()) || (!id.empty() && (id[0] == ',' || (id[0] == ' ' && id[1] == ',') || bPossibleDisplaySyntax )) ) {
			
			//On , look up last from before a (.
			int foundends = 0;
			while (cpos.mColumn > 0 ) { //&& line[cpos.mColumn].mChar != '('

				if (line[cpos.mColumn].mChar == ')')
					foundends++;

				if (line[cpos.mColumn].mChar == '(' && foundends-- <= 0)
					break;

				cpos.mColumn--;
			}


			if (cpos.mColumn > 0) {
				bSymbolLookupActive = true;
				bLookupReadOnly = true;
				dontsuggestword = false;
				if (bEnableFreezeInNextRun) {
					bFreezeWord = false; //Get another one.
				}
				id = GetWordAt(Coordinates(cpos.mLine, cpos.mColumn - 1));

				if (!id.empty() && (id[0] == '(' || id[0] == ' ') && cpos.mColumn > 2 ) {
					id = GetWordAt(Coordinates(cpos.mLine, cpos.mColumn - 2));
					if (!id.empty() && (id[0] == '(' || id[0] == ' ') && cpos.mColumn > 3 ) {
						id = GetWordAt(Coordinates(cpos.mLine, cpos.mColumn - 3));
					}
				}

				bEnableFreezeInNextRun = true;
			}
			else {
				if(bEnableFreezeInNextRun) {
					cpos = mState.mCursorPosition;
					if (cpos.mColumn > 0)
						cpos.mColumn--;
					id = GetWordAt(Coordinates(cpos.mLine, cpos.mColumn - 1));
					bSymbolLookupActive = false;
					bLookupReadOnly = true;
					dontsuggestword = false;
					bFreezeWord = false; //Get another one.
				}
			}
		}
		else if (!id.empty() && (id[0] == '(' || id[0] == ' ') ) {
			//Syntax.
			if (cpos.mColumn > 1) {
				bSymbolLookupActive = false;
				bLookupReadOnly = true;
				dontsuggestword = true;
				id = GetWordAt(Coordinates(cpos.mLine, cpos.mColumn - 1));
				if (id.empty() || (!id.empty() && (id[0] == '(' || id[0] == ' '))) {
					id = GetWordAt(Coordinates(cpos.mLine, cpos.mColumn - 2));
					dontsuggestword = true;

					if (bFreezeWord && freezeWord.empty()) {
						bFreezeWord = false;
						bSymbolLookupActive = true;
						bEnableFreezeInNextRun = true;
					}
				}
			}
		}
		else if (bFreezeWord && !id.empty() && id.length() >= 3 ) {
			//Typed something enable suggestions again.
			bFreezeWord = false;
		}




		if (!bFreezeWord) {
			freezeWord.clear();
			if ( !bTypesLookupActive && (id.empty() || id.length() < 3) ) {
				id.clear();
				freezeSelection = 0;
			}
			//else if()
			//mColorIndex
			else {
				#define MAXSUGGESTIONS 50
				std::string SuggestKeywords[MAXSUGGESTIONS];
				std::string SuggestKeywordOnly[MAXSUGGESTIONS];
				int suggestCount = 0;
				std::transform(id.begin(), id.end(), id.begin(), ::tolower);
				float cx = TextDistanceToLineStart(mState.mCursorPosition);
				float longestwordpx = 0;
				lineNo = (int)floor(scrollY / mCharAdvance.y);

				int curline = mState.mCursorPosition.mLine;
				int place_on_line = curline - lineNo;

				//PE: Support popup with code folding enabled above us.
				if (pref.iEnableCodeFolding && lineNo < MAXPRECHECKLINES) {
					lineNo = m_iNewLineNo[lineNo];
					place_on_line = 0;
					int i = lineNo;
					while (i++ < curline && place_on_line < 120 ) {
						if( m_bLineVisible[i])
							place_on_line++;
					}
				}

				freezePos = ImVec2(cx + 20.0 - scrollX, ( (place_on_line) + 1) * mCharAdvance.y  );

				//Find word syntax.
				int index = tolower(char(id[0]));
				sKeyNext = sKeywordHelp[index];

				if (bTypesLookupActive) {
					//Locate a type.
					std::transform(idleft.begin(), idleft.end(), idleft.begin(), ::tolower);
					freezeWord.clear();
					freezeWord = "";
					
					//Locate the type of a variable.
					if (bSymbolLookupActive && !symbolsCollectList[iCollectListActive].empty()) {
						uString szFoundType = "";
						for (std::map<std::string, TextEditor::SymbolsValues>::iterator it = symbolsCollectList[iCollectListActive].begin(); it != symbolsCollectList[iCollectListActive].end(); ++it)
						{
							//Only variables.
							if (it->second.iSymbolType == 2) {
								//cCommand
								if (strcmp(it->first.c_str(), idleft.c_str()) == 0) {
									//Fullmatch.
									szFoundType = it->second.cType;
									szFoundType.Append(".");
									break;
								}
							}
						}
						if (szFoundType.GetLength() > 0) {

							for (std::map<std::string, TextEditor::SymbolsValues>::iterator it = symbolsCollectList[iCollectListActive].begin(); it != symbolsCollectList[iCollectListActive].end(); ++it)
							{
								//Only Types
								if (it->second.iSymbolType == 4) {

									if (strncmp(it->first.c_str(),szFoundType.GetStr(), szFoundType.GetLength() ) == 0) {
										//Found a type, add to list.
										uString szTmp = it->second.cCommand;
										uString szMemberOnly;
										szTmp.SubString(szMemberOnly, szFoundType.GetLength());

										if (suggestCount < MAXSUGGESTIONS) {
											SuggestKeywords[suggestCount] = szMemberOnly.GetStr();
											SuggestKeywordOnly[suggestCount] = szMemberOnly.GetStr();
											SuggestKeywords[suggestCount].append(it->second.cParameters);
											float pxsize = ImGui::CalcTextSize(SuggestKeywords[suggestCount].c_str()).x;
											if (pxsize > longestwordpx)
												longestwordpx = pxsize;
											suggestCount++;
										}
									}
//									if (strcmp(it->first.c_str(), id.c_str()) == 0) {
//										freezeWord.append(it->second.cCommand);
//										freezeWord.append(it->second.cParameters);
//										freezeWord.append("\n");
//									}


								}
							}

						}
					}

				}
				else if (sKeyNext && sKeyNext->m_pNext) {
					sKeyNext = sKeyNext->m_pNext;
					freezeWord.clear();
					freezeWord = "";
					while (sKeyNext) {

						if (strncmp(sKeyNext->m_cLowerCommand, id.c_str(), id.length() ) == 0) {
							if (suggestCount < MAXSUGGESTIONS) {
								SuggestKeywords[suggestCount] = sKeyNext->m_cCommand;
								SuggestKeywordOnly[suggestCount] = sKeyNext->m_cCommand;
								SuggestKeywords[suggestCount].append(sKeyNext->m_cSyntax.GetStr());

								float pxsize = ImGui::CalcTextSize(SuggestKeywords[suggestCount].c_str()).x;
								if (pxsize > longestwordpx )
									longestwordpx = pxsize;
								suggestCount++;
							}
						}

						if( strcmp(sKeyNext->m_cLowerCommand, id.c_str()) == 0) {
							//Only display one syntax for now.
							if (sKeyNext->m_cReturn.GetLength() > 0) {
								freezeWord.append(sKeyNext->m_cReturn.GetStr());
								freezeWord.append(" = ");
							}
							freezeWord.append(sKeyNext->m_cCommand);
							freezeWord.append(sKeyNext->m_cSyntax.GetStr());
							freezeWord.append("\n");
							//break;
						}
						sKeyNext = sKeyNext->m_pNext;
					}

					//Suggest keywords from collected Symbols lists.

					if (bSymbolLookupActive && suggestCount < MAXSUGGESTIONS) {
						//Add additional suggestions based on source functions,variables,constants
						if (!symbolsCollectList[iCollectListActive].empty()) {
							for (std::map<std::string, TextEditor::SymbolsValues>::iterator it = symbolsCollectList[iCollectListActive].begin(); it != symbolsCollectList[iCollectListActive].end(); ++it)
							{
								//cCommand
								if (strncmp(it->first.c_str(), id.c_str(), id.length()) == 0) {

									if (suggestCount < MAXSUGGESTIONS) {

										SuggestKeywords[suggestCount] = it->second.cCommand;
										SuggestKeywordOnly[suggestCount] = it->second.cCommand;
										//Add Space on constants.
										if( it->second.iSymbolType == 3 )
											SuggestKeywords[suggestCount].append(" ");
										SuggestKeywords[suggestCount].append( it->second.cParameters );
										float pxsize = ImGui::CalcTextSize(SuggestKeywords[suggestCount].c_str()).x;
										if (pxsize > longestwordpx)
											longestwordpx = pxsize;
										suggestCount++;

									}
								}
								if (strcmp(it->first.c_str(), id.c_str()) == 0) {
									//if (sKeyNext->m_cReturn.GetLength() > 0) {
									//	freezeWord.append(sKeyNext->m_cReturn.GetStr());
									//	freezeWord.append(" = ");
									//}
									freezeWord.append(it->second.cCommand);
									freezeWord.append(it->second.cParameters);
									freezeWord.append("\n");
								}

							}
						}

					}
				}

				//popup suggestions.
				if (suggestCount > 0 && freezeWord.empty() && mState.mCursorPosition.mColumn > 1 && !dontsuggestword ) {
					//Suggest commands.
					bSuggestActice = true;
					ImVec2 oldCursor = ImGui::GetCursorPos();
					int maxSuggest = suggestCount;
					if (maxSuggest > 20)
						maxSuggest = 20;

					float wheight = ImGui::GetWindowSize().y; //(ImGui::GetWindowPos().y + ImGui::GetWindowSize().y);
					if (freezePos.y + ((maxSuggest +1)* mCharAdvance.y + 6.0) > wheight )
					{
						freezePos = ImVec2(cx + 20.0 - scrollX, ( ( (place_on_line) ) * mCharAdvance.y) - ((maxSuggest)* mCharAdvance.y + 18.0));
					}

					ImGui::SetNextWindowPos(freezePos + ImGui::GetWindowPos());
					if (suggestCount > 20) {
						ImGui::SetNextWindowSize(ImVec2(0,20* mCharAdvance.y+ 6.0) );
					}

					ImVec2 wsize = ImVec2(longestwordpx+38.0, (maxSuggest) * mCharAdvance.y + 6.0);
					//ImGuiWindowFlags_NoInputs
					//ImGuiWindowFlags_Tooltip
					//ImGuiWindowFlags_NoScrollbar
					ImGuiWindowFlags flags = ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
					ImGui::Begin("##SuggestionsCommand",NULL,flags);
					ImGui::PopStyleVar();
					//ImGui::BeginChild("##SuggestCommand", wsize ); //, flags);
					ImVec4 usecol = ImVec4(style_colors[ImGuiCol_MenuBarBg]);
					usecol.w = 1.0; //I little transparent.

					ImGui::GetWindowDrawList()->AddRectFilled( ImGui::GetWindowPos(), ImGui::GetWindowPos() + wsize , ImGui::GetColorU32( usecol ));

					if (bCursorKeys != 0) {
						freezeSelection += bCursorKeys;
						if (freezeSelection < 0) freezeSelection = suggestCount-1;
						if (freezeSelection < 0) freezeSelection = 0;
						if (freezeSelection >= suggestCount) freezeSelection = 0;
					}
					bool bScroolYSet = false;
					for (int i = 0; i < suggestCount; i++) {

						ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 2, ImGui::GetCursorPos().y ));
						if (bLookupReadOnly) {
							ImGui::MenuItem(SuggestKeywords[i].c_str(),"",false);
						}
						else {
							if (i == freezeSelection) {

								ImVec4 usecol = ImVec4(style_colors[ImGuiCol_ButtonHovered]);
								usecol.w = 1.0;
								ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos() + ImVec2(0, freezeSelection*mCharAdvance.y-ImGui::GetScrollY() ), ImGui::GetWindowPos() + ImVec2(wsize.x, (freezeSelection + 1)*mCharAdvance.y - ImGui::GetScrollY()), ImGui::GetColorU32(usecol));

								if (ImGui::MenuItem(SuggestKeywords[i].c_str(), "", true) || bTabPressed) {

									if (bTypesLookupActive) {
										//This is a type member , just paste at cursor position.
										PasteString((char *)SuggestKeywordOnly[i].c_str());
									}
									else {
										mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = SanitizeCoordinates(Coordinates(mState.mCursorPosition.mLine, mState.mCursorPosition.mColumn - 1));
										if (mSelectionMode == SelectionMode::Line)
											mSelectionMode = SelectionMode::Normal;
										else
											mSelectionMode = SelectionMode::Word;
										SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);
										PasteString((char *)SuggestKeywordOnly[i].c_str());
									}
									break;
								}
							}
							else {
								if (ImGui::MenuItem(SuggestKeywords[i].c_str())) {
									if (bTypesLookupActive) {
										//This is a type member , just paste at cursor position.
										PasteString((char *)SuggestKeywordOnly[i].c_str());
									}
									else {
										mState.mCursorPosition = mInteractiveStart = mInteractiveEnd = SanitizeCoordinates(Coordinates(mState.mCursorPosition.mLine, mState.mCursorPosition.mColumn - 1));
										if (mSelectionMode == SelectionMode::Line)
											mSelectionMode = SelectionMode::Normal;
										else
											mSelectionMode = SelectionMode::Word;
										SetSelection(mInteractiveStart, mInteractiveEnd, mSelectionMode);
										PasteString((char *)SuggestKeywordOnly[i].c_str());
									}
									break;
								}
							}

						}

						if (!bScroolYSet) {
							if (suggestCount >= 20 && freezeSelection >= 15 && i == freezeSelection - 5) {
								ImGui::SetScrollHereY();
								//ImGui::SetScrollY(i*mCharAdvance.y);
								bScroolYSet = true;
							}
						}

					}
//					if (freezeSelection > 15)
//					{
//						ImGui::SetScrollY(freezeSelection*mCharAdvance.y);
//					}
					if (!bScroolYSet) {
						ImGui::SetScrollY(0);
					}
					ImGui::End();
					//ImGui::EndChild();
					ImGui::SetCursorPos(oldCursor);
				}
			}
		}
		if (bEnableFreezeInNextRun && !freezeWord.empty() && freezeWord.length() > 1)
			bFreezeWord = true;
		if (bFreezeWord && !freezeWord.empty() && freezeWord.length() > 1 )
		{
			//popup syntax
			ImVec2 oldCursor = ImGui::GetCursorPos();
			//ImGui::SetCursorPos(freezePos);
			//ImGui::BeginTooltip();
			ImVec2 tpos;

			float wheight = ImGui::GetWindowSize().y; //(ImGui::GetWindowPos().y + ImGui::GetWindowSize().y);
			if (freezePos.y + (2* mCharAdvance.y + 6.0) > wheight)
			{
				freezePos.y -= (4 * mCharAdvance.y + 12.0);
			}

			tpos = freezePos;
			ImGui::SetNextWindowPos(tpos + ImGui::GetWindowPos());
//			ImGui::SetNextWindowPos(freezePos+ImGui::GetWindowPos());
			ImGui::SetNextWindowBgAlpha(style_colors[ImGuiCol_PopupBg].w * 0.80f);
			ImGui::BeginTooltipEx(0, true);
			ImGui::Selectable(freezeWord.c_str());
			ImGui::EndTooltip();
			ImGui::SetCursorPos(oldCursor);
		}
		if (ImGui::IsKeyPressed(27))
		{
			bEnableSuggest = false;
			bSuggestActice = false;
			bFreezeWord = false;
		}

	}
	else
	{
		//bSuggestActice = false;
		//bFreezeWord = false;
	}


	if (mScrollToCursor)
	{
		//Initialised yet ?
		if (ImGui::GetWindowHeight() > 28) { //Default 4 ?
			EnsureCursorVisible();
			if(!bSearchActive) ImGui::SetWindowFocus();
			mScrollToCursor = false;
		}
	}

	//Context menu.

	ImVec2 oldCursor = ImGui::GetCursorPos();
	ImVec2 ws = ImGui::GetWindowSize() - ImVec2(30, 30);
	ImGui::SetCursorPos(ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY()));
	ImGui::Dummy(ws);

	bMouseToWordActive = false;
	bContextActive = false;
	if (ImGui::BeginPopupContextItemAGK(aTitle)) //"project context menu"
	{
		bool ro = IsReadOnly();

		bContextActive = true;

		if (isAGKsource) {

			if (!HasSelection() ) {
				bMouseToWordActive = true;
				auto id = idUnderMouse;
				if (!id.empty())
				{

					uString szClean = id.c_str();
					szClean.Lower();
					int pos = szClean.FindStr("[");
					if(pos <= 0)
						pos = szClean.FindStr(".");
					if (pos > 0) {
						uString tmp;
						szClean.SubString(tmp, 0, pos);
						szClean = tmp;
					}

					uString szCleanFirst = szClean;
					szCleanFirst.Append("##func");
					auto it2 = symbolsList[activeSymbolslist].find(szCleanFirst.GetStr());
					if (it2 == symbolsList[activeSymbolslist].end())
					{
						it2 = symbolsList[activeSymbolslist].find(szClean.GetStr());
					}

					if (it2 != symbolsList[activeSymbolslist].end()) {
						if (ImGui::MenuItem("Go To Definition")) {
							if (it2->second.m_InsideEditor) {

								int gettotop = (winsize.y / mCharAdvance.y) - 10; //-5 lines from top.
								if (gettotop <= 0)
									gettotop = 15;
								mScrollToCursorAddLines = -gettotop;
								mScrollToCursor = true;
								SetCursorPosition(TextEditor::Coordinates(it2->second.lineno - 1, 0));
								mScrollToCursorAddLines = -gettotop;
								mScrollToCursor = true;
							}
						}
					}

					auto it = symbolsCollectList[iCollectListActive].find(szClean.GetStr());
					//
					if (it != symbolsCollectList[iCollectListActive].end())
					{
						if (ImGui::MenuItem("Go To Definition")) {
							if (it->second.m_InsideEditor) {

								cProjectItem::sProjectFiles * m_pCurrentFile;
								//m_pCurrentFile = FindFileFromEditor(it->second.m_InsideEditor);
								m_pCurrentFile = FindFileFromEditorMatchSource(it->second.m_InsideEditor);
								if (m_pCurrentFile) {
									
									if (m_pCurrentFile->m_editor) {

										if (!m_pCurrentFile->m_editor->bEditorVisible) {
											//Make sure its docked.
											m_pCurrentFile->m_editor->firstrun = true;
										}
										m_pCurrentFile->m_editor->bEditorVisible = true;
									}
									
									cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
									bFocusActiveEditor = true;
								}
								//Focus.
								int gettotop = (winsize.y / mCharAdvance.y) - 10; //-5 lines from top.
								if (gettotop <= 0)
									gettotop = 15;
								if (m_pCurrentFile->m_editor) {
									m_pCurrentFile->m_editor->mScrollToCursorAddLines = -gettotop;
									m_pCurrentFile->m_editor->mScrollToCursor = true;
									m_pCurrentFile->m_editor->SetCursorPosition(TextEditor::Coordinates(it->second.lineno - 1, 0));
									m_pCurrentFile->m_editor->mScrollToCursorAddLines = -gettotop;
									m_pCurrentFile->m_editor->mScrollToCursor = true;
								}
								else {
									it->second.m_InsideEditor->mScrollToCursorAddLines = -gettotop;
									it->second.m_InsideEditor->mScrollToCursor = true;
									it->second.m_InsideEditor->SetCursorPosition(TextEditor::Coordinates(it->second.lineno - 1, 0));
									it->second.m_InsideEditor->mScrollToCursorAddLines = -gettotop;
									it->second.m_InsideEditor->mScrollToCursor = true;
								}
							}
						}
					}

				}
			}

			if (HasSelection()) {
				uString szClean = GetSelectedText().c_str();
				szClean.Lower();
				//Prefer functions.
				szClean.Append("##func");
				auto it2 = symbolsList[activeSymbolslist].find(szClean.GetStr());
				if (it2 == symbolsList[activeSymbolslist].end())
				{
					szClean = GetSelectedText().c_str();
					it2 = symbolsList[activeSymbolslist].find(szClean.GetStr());
				}
				if (it2 != symbolsList[activeSymbolslist].end()) {
					if (ImGui::MenuItem("Go To Definition")) {
						if (it2->second.m_InsideEditor) {
							//Focus.
							int gettotop = (winsize.y / mCharAdvance.y) - 10; //-5 lines from top.
							if (gettotop <= 0)
								gettotop = 15;
							mScrollToCursorAddLines = -gettotop;
							mScrollToCursor = true;
							SetCursorPosition(TextEditor::Coordinates(it2->second.lineno - 1, 0));
							mScrollToCursorAddLines = -gettotop;
							mScrollToCursor = true;
						}
					}
				}
				else {

					auto it = symbolsCollectList[iCollectListActive].find(szClean.GetStr());
					//


					if (it != symbolsCollectList[iCollectListActive].end())
					{
						if (ImGui::MenuItem("Go To Definition")) {
							if (it->second.m_InsideEditor) {

								cProjectItem::sProjectFiles * m_pCurrentFile;
								//m_pCurrentFile = FindFileFromEditor(it->second.m_InsideEditor);
								m_pCurrentFile = FindFileFromEditorMatchSource(it->second.m_InsideEditor);
								if (m_pCurrentFile) {

									if (m_pCurrentFile->m_editor) {
										if (!m_pCurrentFile->m_editor->bEditorVisible) {
											//Make sure its docked.
											m_pCurrentFile->m_editor->firstrun = true;
										}
										m_pCurrentFile->m_editor->bEditorVisible = true;
									}

									cNextWindowFocus = (char *)m_pCurrentFile->m_sEditName.GetStr();
									bFocusActiveEditor = true;
								}
								//Focus.
								int gettotop = (winsize.y / mCharAdvance.y) - 10; //-5 lines from top.
								if (gettotop <= 0)
									gettotop = 15;
								if (m_pCurrentFile->m_editor) {
									m_pCurrentFile->m_editor->mScrollToCursorAddLines = -gettotop;
									m_pCurrentFile->m_editor->mScrollToCursor = true;
									m_pCurrentFile->m_editor->SetCursorPosition(TextEditor::Coordinates(it->second.lineno - 1, 0));
									m_pCurrentFile->m_editor->mScrollToCursorAddLines = -gettotop;
									m_pCurrentFile->m_editor->mScrollToCursor = true;
								}
								else {
									it->second.m_InsideEditor->mScrollToCursorAddLines = -gettotop;
									it->second.m_InsideEditor->mScrollToCursor = true;
									it->second.m_InsideEditor->SetCursorPosition(TextEditor::Coordinates(it->second.lineno - 1, 0));
									it->second.m_InsideEditor->mScrollToCursorAddLines = -gettotop;
									it->second.m_InsideEditor->mScrollToCursor = true;
								}

							}
						}
					}
				}
				if (ImGui::MenuItem("Find References")) {

					if (pCurrentSelectedProject) {
						std::string searchfor = GetSelectedText();
						char firstentry[1024];
						ClearAllEditorMarkers();
						ClearCompilerLog();
						uString FocusName;
						cProjectItem::sProjectFiles * allProjectFiles;
						cProjectItem * allMyProjects;
						int replaced = 0;
						allMyProjects = allProjects;
						TextEditor *current_active = m_ActiveEditor;

						while (allMyProjects) {
							allProjectFiles = allMyProjects->m_pFirstFile;
							while (allProjectFiles) {

								if (allMyProjects == pCurrentSelectedProject) {

									if (allProjectFiles->m_editor == current_active) {
										//Focus this.
										FocusName = allProjectFiles->m_sEditName;
									}

									if (allProjectFiles->m_bOpenForEdit && allProjectFiles->m_editor) {


										Coordinates oldpos = allProjectFiles->m_editor->mState.mCursorPosition;
										//Start replace all at the top.
										allProjectFiles->m_editor->mState.mCursorPosition.mLine = 0;
										allProjectFiles->m_editor->mState.mCursorPosition.mColumn = 0;

										allProjectFiles->m_editor->bFindNextEndOfLine = false;

										strcpy(allProjectFiles->m_editor->cnSearch, searchfor.c_str());

										while (!allProjectFiles->m_editor->bFindNextEndOfLine) {
											allProjectFiles->m_editor->FindNext();
											if (!allProjectFiles->m_editor->bFindNextEndOfLine && allProjectFiles->m_editor->HasSelection()) {
												std::string gst;
												auto oldSelEnd = allProjectFiles->m_editor->mState.mSelectionEnd;
												auto oldSelStart = allProjectFiles->m_editor->mState.mSelectionStart;

												const auto& line = allProjectFiles->m_editor->mLines[allProjectFiles->m_editor->mState.mSelectionStart.mLine];
												allProjectFiles->m_editor->mState.mSelectionStart.mColumn = 0;
												allProjectFiles->m_editor->mState.mSelectionEnd.mColumn = line.size();
												gst = allProjectFiles->m_editor->GetSelectedText();

												allProjectFiles->m_editor->mState.mSelectionEnd = oldSelEnd;
												allProjectFiles->m_editor->mState.mSelectionStart = oldSelStart;

												try {
													//Trim gst.
													// trim leading spaces
													size_t startpos = gst.find_first_not_of(" \t");
													if (std::string::npos != startpos)
													{
														gst = gst.substr(startpos);
													}

													AddToCompilerLog(0, "%s:%d:Search:%d:%d: %s\n", allProjectFiles->m_sName.GetStr(), allProjectFiles->m_editor->mState.mSelectionStart.mLine + 1, allProjectFiles->m_editor->mState.mSelectionStart.mColumn, allProjectFiles->m_editor->mState.mSelectionEnd.mColumn, gst.c_str());
													if (replaced == 0) {
														sprintf(firstentry, "%s:%d:Search:%d:%d: %s\n", allProjectFiles->m_sName.GetStr(), allProjectFiles->m_editor->mState.mSelectionStart.mLine + 1, allProjectFiles->m_editor->mState.mSelectionStart.mColumn, allProjectFiles->m_editor->mState.mSelectionEnd.mColumn, gst.c_str());
													}
													replaced++;
												}
												catch (std::exception & e)
												{
													//Ignore exceptions.
												}


											}
											allProjectFiles->m_editor->mScrollToCursor = false;
										}

										allProjectFiles->m_editor->bFindNextEndOfLine = false;
										allProjectFiles->m_editor->mState.mCursorPosition = oldpos;

									}

								}
								allProjectFiles = allProjectFiles->m_pNext;
							}

							allMyProjects = allMyProjects->m_pNext;
						}

						if (replaced > 0) {
							//SetFocus.
							ImGui::SetWindowFocus("Message window");
							//ParseCompilerLog(firstentry, false);
							error_log_scroll_to_top();
						}
						//sprintf(ReplaceMsg, "Found %d times.", replaced);
						bFindNextEndOfLine = false;
						bFindPrevStartOfLine = false;

						if (FocusName.GetLength() > 0)
							szNextWindowFocus.SetStr(FocusName);
						cNextWindowFocus = (char *)szNextWindowFocus.GetStr();
						bFocusActiveEditor = true;
						//							ImGui::SetWindowFocus(FocusName.GetStr());

					}
				}



				if (ImGui::MenuItem("Find References in All Project Files")) {


					if (pCurrentSelectedProject) {
						std::string searchfor = GetSelectedText();
						char firstentry[1024];
						ClearAllEditorMarkers();
						ClearCompilerLog();
						uString FocusName;
						cProjectItem::sProjectFiles * allProjectFiles;
						cProjectItem * allMyProjects;
						int replaced = 0;
						allMyProjects = allProjects;
						TextEditor *current_active = m_ActiveEditor;

						while (allMyProjects) {
							allProjectFiles = allMyProjects->m_pFirstFile;
							while (allProjectFiles) {

								if (allMyProjects == pCurrentSelectedProject) {

									if (allProjectFiles->m_editor == current_active) {
										//Focus this.
										FocusName = allProjectFiles->m_sEditName;
									}
									bool justloaded = false;
									if (!allProjectFiles->m_bOpenForEdit) {
										//We need to open it for replace to work.
										strcpy(cnSearch, cSearch);
										strcpy(cnSearchReplace, cSearchReplace);
										vTextEditorInit(allProjectFiles->m_editor, (char *)allProjectFiles->m_sFullPath.GetStr());
										allProjectFiles->m_bOpenForEdit = true;
										justloaded = true;
										strcpy(cSearch, cnSearch);
										strcpy(cSearchReplace, cnSearchReplace);
									}
									int tmpreplaced = replaced;

									if (allProjectFiles->m_bOpenForEdit && allProjectFiles->m_editor) {


										Coordinates oldpos = allProjectFiles->m_editor->mState.mCursorPosition;
										//Start replace all at the top.
										allProjectFiles->m_editor->mState.mCursorPosition.mLine = 0;
										allProjectFiles->m_editor->mState.mCursorPosition.mColumn = 0;

										allProjectFiles->m_editor->bFindNextEndOfLine = false;

										strcpy(allProjectFiles->m_editor->cnSearch, searchfor.c_str() );

										while (!allProjectFiles->m_editor->bFindNextEndOfLine) {
											allProjectFiles->m_editor->FindNext();
											if (!allProjectFiles->m_editor->bFindNextEndOfLine && allProjectFiles->m_editor->HasSelection()) {
												std::string gst;
												auto oldSelEnd = allProjectFiles->m_editor->mState.mSelectionEnd;
												auto oldSelStart = allProjectFiles->m_editor->mState.mSelectionStart;

												const auto& line = allProjectFiles->m_editor->mLines[allProjectFiles->m_editor->mState.mSelectionStart.mLine];
												allProjectFiles->m_editor->mState.mSelectionStart.mColumn = 0;
												allProjectFiles->m_editor->mState.mSelectionEnd.mColumn = line.size();
												gst = allProjectFiles->m_editor->GetSelectedText();

												allProjectFiles->m_editor->mState.mSelectionEnd = oldSelEnd;
												allProjectFiles->m_editor->mState.mSelectionStart = oldSelStart;
												
												try {
													//Trim gst.
													// trim leading spaces
													size_t startpos = gst.find_first_not_of(" \t");
													if (std::string::npos != startpos)
													{
														gst = gst.substr(startpos);
													}

													AddToCompilerLog(0, "%s:%d:Search:%d:%d: %s\n", allProjectFiles->m_sName.GetStr(), allProjectFiles->m_editor->mState.mSelectionStart.mLine + 1, allProjectFiles->m_editor->mState.mSelectionStart.mColumn, allProjectFiles->m_editor->mState.mSelectionEnd.mColumn, gst.c_str());
													if (replaced == 0) {
														sprintf(firstentry, "%s:%d:Search:%d:%d: %s\n", allProjectFiles->m_sName.GetStr(), allProjectFiles->m_editor->mState.mSelectionStart.mLine + 1, allProjectFiles->m_editor->mState.mSelectionStart.mColumn, allProjectFiles->m_editor->mState.mSelectionEnd.mColumn, gst.c_str());
													}
													replaced++;

												}
												catch (std::exception & e)
												{
													//Ignore exceptions.
												}

											}
										}

										allProjectFiles->m_editor->bFindNextEndOfLine = false;
										allProjectFiles->m_editor->mState.mCursorPosition = oldpos;
										allProjectFiles->m_editor->mScrollToCursor = false;

									}

									if (justloaded && tmpreplaced == replaced) {
										//Just close it again. nothing was found.
										allProjectFiles->m_editor->quiteditor = true;
									}

								}
								allProjectFiles = allProjectFiles->m_pNext;
							}

							allMyProjects = allMyProjects->m_pNext;
						}

						if (replaced > 0) {
							//SetFocus.
							ImGui::SetWindowFocus("Message window");
							//ParseCompilerLog(firstentry, false);
							error_log_scroll_to_top();
						}
						//sprintf(ReplaceMsg, "Found %d times.", replaced);
						bFindNextEndOfLine = false;
						bFindPrevStartOfLine = false;

						if (FocusName.GetLength() > 0)
							szNextWindowFocus.SetStr(FocusName);
							cNextWindowFocus = (char *)szNextWindowFocus.GetStr();
							bFocusActiveEditor = true;
//							ImGui::SetWindowFocus(FocusName.GetStr());

					}
				}

				if (pCurrentSelectedProject) {
					if (ImGui::MenuItem("Add watch")) {

						if (strlen(GetSelectedText().c_str()) < MAX_PATH) {
							addWatch(GetSelectedText().c_str());
						}
					}
					if (ImGui::MenuItem("Delete watch")) {

						if (strlen(GetSelectedText().c_str()) < MAX_PATH) {
							deleteWatch(GetSelectedText().c_str());
						}
					}
				}

				if (ImGui::MenuItem("Indent Block", "Tab")) {
					EnterCharacter('\t', false);
				}
				if (ImGui::MenuItem("Outdent Block", "Shift+Tab")) {
					EnterCharacter('\t', true);
				}

				if (ImGui::MenuItem("Help", "F1")) {
					pref.bDisplayHelpWindow = true;
					Help();
				}
				
				ImGui::Separator();

			}

#ifdef AGK_WINDOWS
			
			if (ImGui::MenuItem("Command Help")) {

				BrowserHelp();

			}
			ImGui::Separator();
#endif	

			if (ImGui::MenuItem("Bookmark Toggle")) {
				//if (bmarks.count(mState.mCursorPosition.mLine + 1) != 0)
				if (mBookMarks.count(mState.mCursorPosition.mLine + 1) != 0)
				{
					//Already there , remove.
//					bmarks.erase(mState.mCursorPosition.mLine + 1);
//					SetBookMarks(bmarks);
					mBookMarks.erase(mState.mCursorPosition.mLine + 1);
					SetBookMarks(mBookMarks);
				}
				else {
					//Not set , Add
//					bmarks.insert(mState.mCursorPosition.mLine + 1);
//					SetBookMarks(bmarks);
					mBookMarks.insert(mState.mCursorPosition.mLine + 1);
					SetBookMarks(mBookMarks);
				}
			}


			if (ImGui::MenuItem("Breakpoint Toggle")) {
				if (bpts.count(mState.mCursorPosition.mLine + 1) != 0)
				{
					//Already there , remove.
					//bpts.find()

					bpts.erase(mState.mCursorPosition.mLine + 1);
					SetBreakpoints(bpts);
					cProjectItem::sProjectFiles * m_pCurrentFile;
					m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);
					if (m_pCurrentFile) {
						char tmp[MAX_PATH];
						sprintf(tmp, "delete breakpoint %s:%d", m_pCurrentFile->m_sName.GetStr(), mState.mCursorPosition.mLine + 1);
						uString bc_err;
						BroadcasterCommand(tmp,bc_err);
					}
					//addAllBreakPoints(NULL);
				}
				else {
					//Not set , Add
					//bpts.clear();
					bpts.insert(mState.mCursorPosition.mLine + 1);
					SetBreakpoints(bpts);
					cProjectItem::sProjectFiles * m_pCurrentFile;
					m_pCurrentFile = FindFileFromEditor(m_ActiveEditor);

					bool bAddRelative = false;
					uString relative;
					if (m_pCurrentFile) {

						/*
						//Cant do this at the moment, need another way.
						//
						if (pCurrentSelectedProject) {

							uString fname = m_pCurrentFile->m_sFullPath;
							fname.ReplaceStr("\\", "/");
							relative = fname;

							uString pfolder = ";:;";
							uString projectpath = pCurrentSelectedProject->m_sProjectFileFullPath;
							projectpath.ReplaceStr("\\", "/");
							int pos = projectpath.RevFindStr("/");
							if (pos > 0) {
								projectpath.SubString(pfolder, 0, pos + 1);
							}

							cProjectItem::sProjectFiles *m_pSearchFile;

							relative.ReplaceStr(pfolder, "");
							bAddRelative = true;
							//resolve to relative path.
							uString szCompares = "";
							int countslash = pfolder.Count('/');
							int i, slashpos = 0;
							if (pfolder[0] != relative[0]) {
								i = 0;
							}
							else {
								for (i = 0; i < countslash; i++) {
									slashpos = pfolder.FindStr("/", 1, slashpos);
									if (slashpos >= 0) {
										if (strncmp(pfolder.GetStr(), relative.GetStr(), slashpos) == 0) {
											pfolder.SubString(szCompares, 0, slashpos);
											slashpos++;
										}
										else {
											break;
										}
									}
									else {
										break;
									}
								}
							}
							//i==0 Must be on another drive. so keep name as is.
							if (i > 0 && i < countslash) {
								//Make relative.
								int iRemainingSlash = countslash - i;
								szCompares.Append("/"); //Add missing /
								relative.ReplaceStr(szCompares, ""); // remove similar path.
								uString szFinal = "";
								for (int iAddBackPath = 0; iAddBackPath < iRemainingSlash; iAddBackPath++) {
									szFinal.Append("../");
								}
								szFinal.Append(relative);
								relative = szFinal;
							}
						}
						*/
						char tmp[MAX_PATH];
						if (bAddRelative) {
							sprintf(tmp, "breakpoint %s:%d", relative.GetStr(), mState.mCursorPosition.mLine + 1);
							uString bc_err;
							BroadcasterCommand(tmp,bc_err);
						}
						else {
							sprintf(tmp, "breakpoint %s:%d", m_pCurrentFile->m_sName.GetStr(), mState.mCursorPosition.mLine + 1);
							uString bc_err;
							BroadcasterCommand(tmp,bc_err);
						}
					}
					//addAllBreakPoints(NULL);
				}
			}
		}

		if (ImGui::MenuItem("Read-Only Mode", nullptr, &ro))
			SetReadOnly(ro);

		ImGui::Separator();

		if (ImGui::MenuItem("Find", pref.cFindText) ) {
			OpenFind();
		}

		if (ImGui::MenuItem("Replace", pref.cReplaceText)) {
			Replace();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Toggle Comment Line ", "Ctrl-E", nullptr, !ro )) {
			ToggleLineComments();
		}


		if (ImGui::MenuItem("Undo", pref.cUndoText, nullptr, !ro && CanUndo())) {
			Undo();
			mScrollToCursor = true;
		}
		if (ImGui::MenuItem("Redo", pref.cRedoText, nullptr, !ro && CanRedo())) {
			Redo();
			mScrollToCursor = true;
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Copy", pref.cCopyText, nullptr, HasSelection()))
			Copy();
		if (ImGui::MenuItem("Cut", pref.cCutText, nullptr, !ro && HasSelection()))
			Cut();
		//if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && HasSelection()))
		//	Delete();
		if (ImGui::MenuItem("Paste", pref.cPasteText, nullptr, !ro && ImGui::GetClipboardText() != nullptr))
			Paste();

		ImGui::Separator();

		if (ImGui::MenuItem("Select All", nullptr, nullptr))
			SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(GetTotalLines(), 0));
		
			ImGui::Separator();

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Default Style"))
				SetPalette(TextEditor::GetAGKPalette());
			if (ImGui::MenuItem("Dark Style"))
				SetPalette(TextEditor::GetDarkPalette());
			if (ImGui::MenuItem("Light Style"))
				SetPalette(TextEditor::GetLightPalette());
			if (ImGui::MenuItem("Retro Blue Style"))
				SetPalette(TextEditor::GetRetroBluePalette());

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}
	else {
		bFreezeUnderMouse = false;
	}

	ImGui::SetCursorPos(oldCursor);

	if (mScrollToCursor)
	{
		//Initialised yet ?
		if (ImGui::GetWindowHeight() > 28) { //Default 4 ?
			EnsureCursorVisible();
			if(!bSearchActive) ImGui::SetWindowFocus();
			mScrollToCursor = false;
		}
	}


	ImGui::PopAllowKeyboardFocus();
	ImGui::EndChild();
	ImGui::PopStyleVar();
	//ImGui::PopStyleColor();

	mWithinRender = false;
}

void TextEditor::OpenFind(void)
{

	bFreezeWord = false;
	replaceactive = false;
	if (ImGui::GetTime() - lastKeySearch >= 0.125) {
		if (bSearchActive == false)
			ToggleSeachReplace(true);
		else
			ToggleSeachReplace(false);
		lastKeySearch = (float)ImGui::GetTime();
	}
}
void TextEditor::Replace(void)
{
	replaceactive = true;
	bHideReplaceBut = false;
	if (bSearchActive == false)
		ToggleSeachReplace(true);
	else
		ToggleSeachReplace(false);
}

void TextEditor::FindPrev(void)
{
	if (mLines.empty())
		return;

	std::string utf8result;

	char find[MAX_PATH];
	strcpy(find, cSearch);

	std::cmatch results;
	std::string id;

	bFindNextEndOfLine = false;

	int startline = mState.mCursorPosition.mLine;
	int startmColumn = mState.mCursorPosition.mColumn - 1;
	if (startmColumn < 0)
		startmColumn = 0;
	if (startmColumn == 0 && startline > 0)
		startline--;

	if (bFindPrevStartOfLine) {
		startline = mLines.size()-1;
		startmColumn = mLines[startline].size();
		bFindPrevStartOfLine = false;
	}

	for (int i = startline; i >= 0; --i)
	{
		auto& line = mLines[i];

		if (line.empty())
			continue;

		utf8result.clear();
		for (size_t j = 0; j < line.size(); ++j)
		{
			auto& col = line[j];
			if (col.mInt > 0) {

				std::string ms = UnicodeToUTF8(col.mInt);
				for (int a = 0; a < ms.size(); a++) {
					utf8result.push_back(ms[a]);
				}
			}
			else {
				utf8result.push_back(col.mChar);
			}
		}

		char * buffer = (char *)utf8result.c_str();

//		char * buffer = new char[line.size() + 1];
//		for (size_t j = 0; j < line.size(); ++j)
//		{
//			auto& col = line[j];
//			buffer[j] = col.mChar;
//			buffer[j + 1] = 0;
//		}

		if (startmColumn >= line.size())
			startmColumn = line.size();  //PE: removed line.size()-1 : 02-06-2020.
		
		char *found;

		if ( bMatchCase) //replaceactive &&
			found = (char*) strstr(&buffer[0], find);
		else
			found = (char*) pestrcasestr(&buffer[0], find);

		while (bMatchWord && found) {
			char *oldfound = found;
			int pos = found - &buffer[0];
			if (pos == 0) {
				char t = buffer[pos + strlen(find)];
				if (!strchr(WordOnlyAllowBySide, t))
					found = NULL;
//				if( buffer[pos + strlen(find)] != ' ' && buffer[pos + strlen(find)] != '\t' ) {
//					found = NULL;
//				}
			}
			else {
				if (pos + strlen(find) <= strlen(buffer)) {
					char t;
					if (pos + strlen(find) < strlen(&buffer[0])) {
						t = buffer[pos + strlen(find)];
						if (!strchr(WordOnlyAllowBySide, t))
							found = NULL;
					}
					if (pos > 0) {
						t = buffer[pos - 1];
						if (!strchr(WordOnlyAllowBySide, t))
							found = NULL;
					}

//					if (buffer[pos + strlen(find)] != ' ' && buffer[pos + strlen(find)] != '\t') {
//						found = NULL;
//					}
//					if (buffer[pos-1] != ' ' && buffer[pos-1] != '\t') {
//						found = NULL;
//					}
				}
			}
			if (found)
				break;
			if (oldfound && startmColumn > 0 && oldfound >= &buffer[startmColumn])
				break;
			found = oldfound + strlen(find);

			if (bMatchCase) //replaceactive &&
				found = (char*)strstr(found, find);
			else
				found = (char*)pestrcasestr(found, find);
		}

		if (found && startmColumn > 0 && found >= &buffer[startmColumn])
			found = NULL;

		char *found2 = found;
		int iPos = 0;
		while (found2) {
			if (*found + 1 == 0)
				break;

			if (bMatchCase)
				found2 = (char*)strstr(found + 1 + iPos, find);
			else
				found2 = (char*)pestrcasestr(found+1 + iPos, find);

			if (bMatchWord && found2) {
				char * found3 = found2;
				int pos = found2 - &buffer[0];
				if (pos == 0) {
					char t = buffer[pos + strlen(find)];
					if (!strchr(WordOnlyAllowBySide, t))
						found2 = NULL;

//					if (buffer[pos + strlen(find)] != ' ' && buffer[pos + strlen(find)] != '\t') {
//						found2 = NULL;
//					}
				}
				else {
					if (pos + strlen(find) < strlen(buffer)) {
						char t = buffer[pos + strlen(find)];
						if (!strchr(WordOnlyAllowBySide, t))
							found2 = NULL;
						t = buffer[pos - 1];
						if (!strchr(WordOnlyAllowBySide, t))
							found2 = NULL;

//						if (buffer[pos + strlen(find)] != ' ' && buffer[pos + strlen(find)] != '\t') {
//							found2 = NULL;
//						}
//						if (buffer[pos - 1] != ' ' && buffer[pos - 1] != '\t') {
//							found2 = NULL;
//						}
					}
				}

				if (startmColumn > 0 && found3 >= &buffer[startmColumn])
					break;

				if (found2)
					found = found2;
				else {
					found2 = found;
					iPos++;
				}

			}
			else {
				if (!found2)
					break;
				if (startmColumn > 0 && found2 >= &buffer[startmColumn])
					break;
				found = found2;
			}



		}

		startmColumn = 0;

		if (found) {
			//
			int pos = found - &buffer[0];
			int il = pos;

			char utf8count[MAX_PATH];
			if (pos < MAX_PATH - 10) {
				strncpy(utf8count, &buffer[0], pos);
				utf8count[pos] = 0;
				uString stmp = utf8count;
				il = stmp.CountUTF8Chars(utf8count, NULL);
			}

//			delete [] buffer;
			SetCursorPosition(TextEditor::Coordinates(i, pos));


			if (il < pos)
				mState.mSelectionStart = TextEditor::Coordinates(i, il);
			else
				mState.mSelectionStart = TextEditor::Coordinates(i, pos);

			//mState.mSelectionStart = TextEditor::Coordinates(i, pos);

			uString tmp = cSearch;
			int tlength = tmp.CountUTF8Chars(cSearch, NULL);
			if (il < pos)
				mState.mSelectionEnd = TextEditor::Coordinates(i, il + tlength);
			else
				mState.mSelectionEnd = TextEditor::Coordinates(i, pos + tlength);

//			mState.mSelectionEnd = TextEditor::Coordinates(i, pos + strlen(cSearch));
			EnsureCursorVisible();
			mScrollToCursor = true;
			return;
		}
//		delete [] buffer;

	}

	bFindPrevStartOfLine = true;
}

int TextEditor::WordCount(void)
{
	int endLine = mLines.size();
	int startline = 0;
	int wordcount = 0;
	for (int i = startline; i < endLine; ++i)
	{
		auto& line = mLines[i];

		if (line.empty())
			continue;
		if (line.size() > 1) {
			bool nonspacefound = false;
			for (size_t j = 0; j < line.size() - 1; ++j) {
				if (line[j].mChar != ' ')
					nonspacefound = true;
				if( (line[j].mChar == ' '|| line[j].mChar == '\t') && (line[j + 1].mChar != ' ' || line[j + 1].mChar == '\n') ) {
					wordcount++;
				}
			}
			//nl endings.
			if(nonspacefound)
				wordcount++;
		}
	}
	return(wordcount);
}
void TextEditor::ToggleLineComments(void)
{
	if (mLines.empty())
		return;

	int startline = mState.mCursorPosition.mLine;
	int endline = mState.mCursorPosition.mLine;
	bool setOldSelection = false;
	if (HasSelection()) {
		//Multiple line comments.
		startline = mState.mSelectionStart.mLine;
		endline = mState.mSelectionEnd.mLine;
		setOldSelection = true;
	}

	for (int iloop = startline; iloop <= endline; iloop++) {
		auto& line = mLines[iloop];
		if (line.size() > 0) {
			if (endline > startline) {
				mState.mCursorPosition.mLine = iloop;
			}
			if (line.size() >= 3) {
				if (line[0].mChar == '/' && line[1].mChar == '/' && line[2].mChar == '~') {
					//Remove.
					mState.mSelectionStart = mState.mCursorPosition;
					mState.mSelectionStart.mColumn = 0;
					mState.mSelectionEnd = mState.mCursorPosition;
					mState.mSelectionEnd.mColumn = 3;
					Delete(); //PE: Changed to not use cliboard.
				}
				else {
					mState.mSelectionStart = mState.mCursorPosition;
					mState.mSelectionStart.mColumn = 0;
					mState.mSelectionEnd = mState.mCursorPosition;
					mState.mSelectionEnd.mColumn = 0;
					mState.mCursorPosition.mColumn = 0;
					char tmp[10] = "//~\0";
					PasteString(tmp);
				}
			}
			else {
				mState.mSelectionStart = mState.mCursorPosition;
				mState.mSelectionStart.mColumn = 0;
				mState.mSelectionEnd = mState.mCursorPosition;
				mState.mSelectionEnd.mColumn = 0;
				mState.mCursorPosition.mColumn = 0;
				char tmp[10] = "//~\0";
				PasteString(tmp);
			}
		}
	}
	if (setOldSelection) {
		mState.mSelectionStart.mLine = startline;
		mState.mSelectionStart.mColumn = 0;

		mState.mSelectionEnd.mLine = endline;
		mState.mSelectionEnd.mColumn = mLines[endline].size();
	}
}

void TextEditor::FindNext( void )
{
	if (mLines.empty() )
		return;

	char find[MAX_PATH];
	strcpy(find, cnSearch);
	std::cmatch results;
	std::string id;
	std::string utf8result;

	bFindPrevStartOfLine = false;

	int endLine = mLines.size();
	int startline = mState.mCursorPosition.mLine;
	int startmColumn = mState.mCursorPosition.mColumn+1;

	if (mState.mCursorPosition.mLine == 0 && mState.mCursorPosition.mColumn == 0)
	{
		startline = 0;
		startmColumn = 0;
	}
	if (bFindNextEndOfLine) {
		startline = 0;
		startmColumn = 0;
		bFindNextEndOfLine = false;
	}

	for (int i = startline; i < endLine; ++i)
	{
		auto& line = mLines[i];

		if (line.empty())
			continue;

		utf8result.clear();
		for (size_t j = 0; j < line.size(); ++j)
		{
			auto& col = line[j];
			if (col.mInt > 0) {

				std::string ms = UnicodeToUTF8(col.mInt);
				for (int a = 0; a < ms.size(); a++) {
					utf8result.push_back(ms[a]);
				}
			}
			else {
				utf8result.push_back(col.mChar);
			}
		}

		char * buffer = (char *) utf8result.c_str();

//		char * buffer = new char[line.size() + 1];
//		for (size_t j = 0; j < line.size(); ++j)
//		{
//			auto& col = line[j];
//			buffer[j] = col.mChar;
//			buffer[j + 1] = 0;
//		}

		//UTF8: Neded support here.

		if (startmColumn >= line.size())
			startmColumn = line.size(); //PE: removed line.size()-1 : 02-06-2020.
		char *found;
		if(bMatchCase)
			found = (char*) strstr(&buffer[startmColumn],find);
		else
			found = (char*) pestrcasestr(&buffer[startmColumn], find);
		
		while(bMatchWord && found && startmColumn < line.size() - 1 ) {
			int pos = found - &buffer[0];
			if (pos == 0) {
				char t = buffer[pos + strlen(find) + startmColumn];
				if (!strchr(WordOnlyAllowBySide, t))
					found = NULL;

//				if (buffer[pos + strlen(find)+ startmColumn] != ' ' && buffer[pos + strlen(find)+ startmColumn] != '\t') {
//					found = NULL;
//				}
			}
			else {
				if (pos + strlen(find) <= strlen(&buffer[0])) {

					char t;
					if (pos + strlen(find) < strlen(&buffer[0])) {
						t = buffer[pos + strlen(find)];
						if (!strchr(WordOnlyAllowBySide, t))
							found = NULL;
					}
					if (pos > 0) {
						t = buffer[pos - 1];
						if (!strchr(WordOnlyAllowBySide, t))
							found = NULL;
					}

//					if (buffer[pos + strlen(find)] != ' ' && buffer[pos + strlen(find)] != '\t') {
//						found = NULL;
//					}
//					if (buffer[pos - 1] != ' ' && buffer[pos - 1] != '\t') {
//						found = NULL;
//					}
				}
			}
			if (!found) {
				//Try to find another.
				startmColumn += strlen(find);
				if (bMatchCase)
					found = (char*)strstr(&buffer[startmColumn], find);
				else
					found = (char*)pestrcasestr(&buffer[startmColumn], find);
			}
			else {
				break;
			}
		}

		startmColumn = 0;

		if (found) {
			//
			int pos = found - &buffer[0];

			int il = pos;

			char utf8count[MAX_PATH];
			if (pos < MAX_PATH - 10) {
				strncpy(utf8count, &buffer[0], pos);
				utf8count[pos] = 0;
				uString stmp = utf8count;
				il = stmp.CountUTF8Chars(utf8count, NULL);
			}

			//UTF8: pos need to be in single chars not utf8 chars.
			//So substract UTF8 chars from before "found" for selectionstart.


//			delete [] buffer;
			SetCursorPosition(TextEditor::Coordinates( i, pos ));
			if(il < pos)
				mState.mSelectionStart = TextEditor::Coordinates(i, il);
			else
				mState.mSelectionStart = TextEditor::Coordinates(i, pos);

			uString tmp = cnSearch;
			int tlength = tmp.CountUTF8Chars(cnSearch,NULL);
			if (il < pos)
				mState.mSelectionEnd = TextEditor::Coordinates(i, il + tlength );
			else
				mState.mSelectionEnd = TextEditor::Coordinates(i, pos + tlength);

			EnsureCursorVisible();
			mScrollToCursor = true;
			return;
		}
//		delete [] buffer;

	}

	bFindNextEndOfLine = true;

}



int DecodeUTF8Char(const char* str, int *numBytes)
{
	if (numBytes) *numBytes = 1;

	const unsigned char* ustr = (const unsigned char*)str;
	if (*ustr < 128)
	{
		// one byte
		return *ustr;
	}
	else
	{
		int result;

		if (*ustr < 194) return -1; // not valid as the first byte

		if (*ustr < 224)
		{
			// two bytes
			result = (*ustr & 0x1F);
			result <<= 6;

			ustr++;
			if ((*ustr & 0xC0) != 0x80) return -1; // second byte must start 10xxxxxx
			result |= (*ustr & 0x3F);

			if (numBytes) (*numBytes)++;

			return result;
		}

		if (*ustr < 240)
		{
			// three bytes
			result = (*ustr & 0x0F);
			result <<= 6;

			ustr++;
			if ((*ustr & 0xC0) != 0x80) return -1; // second byte must start 10xxxxxx
			result |= (*ustr & 0x3F);
			result <<= 6;

			if (numBytes) (*numBytes)++;

			ustr++;
			if ((*ustr & 0xC0) != 0x80) return -1; // third byte must start 10xxxxxx
			result |= (*ustr & 0x3F);

			if (numBytes) (*numBytes)++;

			if (result < 0x800) return -1; // overlong encoding
			if (result >= 0xD800 && result <= 0xDFFF) return -1; // reserved for UTF-16, not valid characters

			return result;
		}

		if (*ustr < 245)
		{
			// four bytes
			result = (*ustr & 0x07);
			result <<= 6;

			ustr++;
			if ((*ustr & 0xC0) != 0x80) return -1; // second byte must start 10xxxxxx
			result |= (*ustr & 0x3F);
			result <<= 6;

			if (numBytes) (*numBytes)++;

			ustr++;
			if ((*ustr & 0xC0) != 0x80) return -1; // third byte must start 10xxxxxx
			result |= (*ustr & 0x3F);
			result <<= 6;

			if (numBytes) (*numBytes)++;

			ustr++;
			if ((*ustr & 0xC0) != 0x80) return -1; // fourth byte must start 10xxxxxx
			result |= (*ustr & 0x3F);

			if (numBytes) (*numBytes)++;

			if (result < 0x10000) return -1; // overlong encoding
			if (result > 0x10FFFF) return -1; // outside valid character range

			return result;
		}

		// invalid
		return -1;
	}
}

std::string UnicodeToUTF8(unsigned int codepoint)
{
	std::string out;

	if (codepoint <= 0x7f)
		out.append(1, static_cast<char>(codepoint));
	else if (codepoint <= 0x7ff)
	{
		out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
		out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
	}
	else if (codepoint <= 0xffff)
	{
		out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
		out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
		out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
	}
	else
	{
		out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
		out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
		out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
		out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
	}
	return out;
}


void TextEditor::SetText(const std::string & aText)
{
	mLines.clear();
	mLines.emplace_back(Line());
//	memset(&cSearch[0], 0, MAX_PATH);
//	memset(&cSearchReplace[0], 0, MAX_PATH);
	
	//UTF8: Convert everything to codepoints.
	for (std::string::size_type i = 0; i < aText.size(); ++i) {
		char *utfconvert = (char *)aText.c_str() + i;
		unsigned char chr = *utfconvert;

		int numbytes = 0;
		int utfchar = -1;
		if (utfconvert) {

			if (utfconvert < (aText.c_str() + aText.size() - 3)) {
				
				utfchar = DecodeUTF8Char(utfconvert, &numbytes);
			}
		}

		if (chr == '\r')
		{
			// ignore the carriage return character
		}
		else if (chr == '\n')
			mLines.emplace_back(Line());
		else
		{

			//UTF8ToUniCodePoint((char *)clipText);
			if (utfchar != -1 && numbytes > 1) {
				bool mcom = false;
				mLines.back().emplace_back(Glyph(chr, PaletteIndex::Default, mcom, utfchar));
				i += numbytes - 1;
			}
			else {
				//UTF8: convert non utf8 to utf8
				if (utfchar == -1 && chr > 127) {
					char utf8[8];
					if (CharToUtf8(&utf8[0], 2, chr) == 2)
					{
						//Just add as codepoint.
						mLines.back().emplace_back(Glyph(chr, PaletteIndex::Default, false, chr));
					}
					else {
						mLines.back().emplace_back(Glyph(chr, PaletteIndex::Default));
					}
				}
				else {
					mLines.back().emplace_back(Glyph(chr, PaletteIndex::Default));
				}
			}
		}

		mTextChanged = true;

		//do_things_with(str[i]);
	}



	//Works but. would hate to change original code, so dont do this at the moment.
	//Even if UTF8 us used it will work without changing to unicode, its just a visual thing.
	//UTF8ToUniCodePoint( (char *) aText.c_str());


	//int uString::DecodeUTF8Char(const char* str, int *numBytes)

	//DecodeUTF8Char


/* OLD WAY 
	for (auto chr : aText)
	{
		if (chr == '\r')
		{
			// ignore the carriage return character
		}
		else if (chr == '\n')
			mLines.emplace_back(Line());
		else
		{

			//UTF8ToUniCodePoint((char *)clipText);

			mLines.back().emplace_back(Glyph(chr, PaletteIndex::Default));
		}

		mTextChanged = true;
	}
*/
	mUndoBuffer.clear();
	mUndoIndex = 0;

	Colorize();
}

void TextEditor::SetTextLines(const std::vector<std::string> & aLines)
{
	mLines.clear();
	
	if (aLines.empty())
	{
		mLines.emplace_back(Line());
	}
	else
	{
		mLines.resize(aLines.size());
		
		for (size_t i = 0; i < aLines.size(); ++i)
		{
			const std::string & aLine = aLines[i];
			
			mLines[i].reserve(aLine.size());
			for (size_t j = 0; j < aLine.size(); ++j)
				mLines[i].emplace_back(Glyph(aLine[j], PaletteIndex::Default));
		}
	}

	mTextChanged = true;

	mUndoBuffer.clear();
	mUndoIndex = 0;
	Colorize();
}

void TextEditor::EnterCharacter(Char aChar, bool aShift, int codepoint)
{
	assert(!mReadOnly);

	UndoRecord u;

	u.mBefore = mState;
	filechanged = true;
	
	if (HasSelection())
	{
		mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		if (aChar == '\t')
		{
			auto start = mState.mSelectionStart;
			auto end = mState.mSelectionEnd;

			if (start > end)
				std::swap(start, end);
			start.mColumn = 0;
//			end.mColumn = end.mLine < mLines.size() ? mLines[end.mLine].size() : 0;
			if (end.mColumn == 0 && end.mLine > 0)
			{
				--end.mLine;
				end.mColumn = mLines[end.mLine].size();
			}

			u.mRemovedStart = start;
			u.mRemovedEnd = end;
			u.mRemoved = GetText(start, end);

			bool modified = false;

			for (int i = start.mLine; i <= end.mLine; i++)
			{
				auto& line = mLines[i];
				if (aShift)
				{
					if ( line.size() > 0 ) {
						if (line[0].mChar == '\t')
						{
							line.erase(line.begin());
							if (i == end.mLine && end.mColumn > 0)
								end.mColumn--;
							modified = true;
						}
						else for (int j = 0; j < mTabSize && line.size() > 0 && line[0].mChar == ' '; j++)
						{
							line.erase(line.begin());
							if (i == end.mLine && end.mColumn > 0)
								end.mColumn--;
							modified = true;
							if (line.size() <= 0)
								break;
						}
					}
				}
				else
				{
					line.insert(line.begin(), Glyph('\t', TextEditor::PaletteIndex::Background));
					if (i == end.mLine)
						++end.mColumn;
					modified = true;
				}
			}

			if (modified)
			{
				u.mAddedStart = start;
				u.mAddedEnd = end;
				u.mAdded = GetText(start, end);
				
				u.mAfter = mState;

				mTextChanged = true;

				AddUndo(u);
				EnsureCursorVisible();
			}

			return;
		}
		else
		{
			u.mRemoved = GetSelectedText();
			u.mRemovedStart = mState.mSelectionStart;
			u.mRemovedEnd = mState.mSelectionEnd;
			DeleteSelection();
		}
	}

	auto coord = GetActualCursorCoordinates();
	u.mAddedStart = coord;

	assert(!mLines.empty());

	if (aChar == '\n')
	{
		uString sTotalAdded = "\n";
		char cTotalAdded[MAX_PATH];

		int iTotalAdd = 0;
		cTotalAdded[iTotalAdd++] = '\n';
		cTotalAdded[iTotalAdd] = 0;

		lastKeyedWord[0] = 0;
		InsertLine(coord.mLine + 1);
		auto& line = mLines[coord.mLine];
		auto& newLine = mLines[coord.mLine + 1];
		
		if (pref.bAutoIndentation) // mLanguageDefinition.mAutoIndentation
		{
			for (size_t it = 0; it < line.size() && isblank((unsigned char)line[it].mChar); ++it) {
				newLine.push_back(line[it]);
				if (iTotalAdd < MAX_PATH - 1) {
					cTotalAdded[iTotalAdd++] = line[it].mChar;
					cTotalAdded[iTotalAdd] = 0;
				}
			}
		}
		if ( isAGKsource && pref.bSmartAutoIndentation && line.size() > 0 )
		{
			//Add additional [tab] if line contain if,for,while,do...
			lastKeyedWord[0] = ';';
			for (size_t it = 0; it < line.size() ; ++it) {
				if (it < MAX_PATH-2) {
					lastKeyedWord[it+1] = line[it].mChar;
					lastKeyedWord[it + 2] = ';'; //make it easy to parse always end on ;
					lastKeyedWord[it + 3] = 0;
				}
			}
			uString szClean = lastKeyedWord;
			szClean.ReplaceStr("\t", ";");
			szClean.ReplaceStr(" ", ";");
			szClean.ReplaceStr(":", ";");
			szClean.ReplaceStr("\n", ";");

			bool Indent = false;
			char *found = (char *)pestrcasestr(szClean.GetStr(), ";for;");
			if (!found) {
				found = (char *)pestrcasestr(szClean.GetStr(), ";if;");
				if (found) {
					if( pestrcasestr(szClean.GetStr(), ";then;") )
						found = NULL;
				}
			}
			if (!found) found = (char *)pestrcasestr(szClean.GetStr(), ";case;");
			if (!found) found = (char *)pestrcasestr(szClean.GetStr(), ";case default");
			if (!found) found = (char *)pestrcasestr(szClean.GetStr(), ";do;");
			if (!found) found = (char *)pestrcasestr(szClean.GetStr(), ";else;");
			if (!found) found = (char *)pestrcasestr(szClean.GetStr(), ";elseif;");
			if (!found) found = (char *)pestrcasestr(szClean.GetStr(), ";for;");
			if (!found) found = (char *)pestrcasestr(szClean.GetStr(), ";function;");
			if (!found) found = (char *)pestrcasestr(szClean.GetStr(), ";repeat;");
			if (!found) found = (char *)pestrcasestr(szClean.GetStr(), ";select;");
			if (!found) found = (char *)pestrcasestr(szClean.GetStr(), ";type;");
			if (!found) found = (char *)pestrcasestr(szClean.GetStr(), ";while;");
			if (found) {
				int pos = found - szClean.GetStr();
				if (mState.mCursorPosition.mColumn > pos) {
					auto tab = line[0];
					tab.mChar = '\t';
					newLine.push_back(tab);
					if (iTotalAdd < MAX_PATH - 1) {
						cTotalAdded[iTotalAdd++] = tab.mChar;
						cTotalAdded[iTotalAdd] = 0;
					}

				}
			}
			lastKeyedWord[0] = 0;
		}
		
		const size_t whitespaceSize = newLine.size();
		newLine.insert(newLine.end(), line.begin() + coord.mColumn, line.end());
		line.erase(line.begin() + coord.mColumn, line.begin() + line.size());
		SetCursorPosition(TextEditor::Coordinates(coord.mLine + 1, (int)whitespaceSize));
		mScrollToCursor = true;

		mTextChanged = true;

		u.mAdded = cTotalAdded;
		u.mAddedEnd = GetActualCursorCoordinates();
		u.mAfter = mState;

		AddUndo(u);

		Colorize(coord.mLine - 1, 6); //PE: from 3
		EnsureCursorVisible();
		return;
	}
	else
	{
		auto& line = mLines[coord.mLine];

		//PE: Add shift+tab single line support.

		if (aChar == '\t' && aShift) {

			bool modified = false;
			int i = coord.mLine;
			auto start = coord;
			auto end = coord;
			start.mColumn = 0;

			u.mRemovedStart = start;
			u.mRemovedEnd = end;
			u.mRemovedEnd.mColumn = line.size();
			u.mRemoved = GetText(start, u.mRemovedEnd);


			if (line.size() > 0) {
				if (line[0].mChar == '\t')
				{
					line.erase(line.begin());
					if (end.mColumn > 0)
						end.mColumn--;
					modified = true;
				}
				else for (int j = 0; j < mTabSize && line.size() > 0 && line[0].mChar == ' '; j++)
				{
					line.erase(line.begin());
					if (end.mColumn > 0)
						end.mColumn--;
					modified = true;
					if (line.size() <= 0)
						break;
				}

				SetCursorPosition(Coordinates(coord.mLine, end.mColumn ));

			}
			if (modified)
			{
				u.mAddedStart = start;
				u.mAddedEnd = end;
				u.mAddedEnd.mColumn = line.size();
				u.mAdded = GetText(start, u.mAddedEnd);

				u.mAfter = mState;

				mTextChanged = true;

				AddUndo(u);
				EnsureCursorVisible();
			}

			return;

		}
		else {
			//UTF8: need support here.
			if (codepoint > 127) {
				if (coord.mColumn <= line.size()) {
					if (mOverwrite && (int)line.size() > coord.mColumn)
						line[coord.mColumn] = Glyph('?', PaletteIndex::Default, false, codepoint);
					else {
						line.insert(line.begin() + coord.mColumn, Glyph('?', PaletteIndex::Default, false, codepoint));
					}
				}
			}
			else {
				if (coord.mColumn <= line.size()) {
					if (mOverwrite && (int)line.size() > coord.mColumn)
						line[coord.mColumn] = Glyph(aChar, PaletteIndex::Default);
					else
						line.insert(line.begin() + coord.mColumn, Glyph(aChar, PaletteIndex::Default));
				}
			}
			SetCursorPosition(Coordinates(coord.mLine, coord.mColumn + 1));
		}
		//Set lastKeyedWord using line.begin() + coord.mColumn going backward.
		int a,b,c;
		for (a = coord.mColumn; a > 0; a--) {
			if (line[a].mChar == ' ' || line[a].mChar == '\t')
				break;
		}
		c = 0;
		for (b = a; b < coord.mColumn+1; b++) {
			lastKeyedWord[c++] = line[b].mChar;
			lastKeyedWord[c] = 0;
			if (c >= MAX_PATH-1)
				break;
		}
	}

	mTextChanged = true;

	u.mAdded = aChar;
	u.mAddedEnd = GetActualCursorCoordinates();
	u.mAfter = mState;

	AddUndo(u);

	Colorize(coord.mLine - 1, 6); //PE: from 3
	EnsureCursorVisible();
}

void TextEditor::SetReadOnly(bool aValue)
{
	mReadOnly = aValue;
}

void TextEditor::SetCursorPosition(const Coordinates & aPosition)
{
	if (mState.mCursorPosition != aPosition)
	{
		mState.mCursorPosition = aPosition;
		mCursorPositionChanged = true;
		EnsureCursorVisible();
	}
}

void TextEditor::SetSelectionStart(const Coordinates & aPosition)
{
	mState.mSelectionStart = SanitizeCoordinates(aPosition);
	if (mState.mSelectionStart > mState.mSelectionEnd)
		std::swap(mState.mSelectionStart, mState.mSelectionEnd);
}

void TextEditor::SetSelectionEnd(const Coordinates & aPosition)
{
	mState.mSelectionEnd = SanitizeCoordinates(aPosition);
	if (mState.mSelectionStart > mState.mSelectionEnd)
		std::swap(mState.mSelectionStart, mState.mSelectionEnd);
}


void TextEditor::SetSelection(const Coordinates & aStart, const Coordinates & aEnd, SelectionMode aMode, bool MouseSelected)
{
	if (MouseSelected)
		selectedfrommouse = true;
	else
		selectedfrommouse = false;
	auto oldSelStart = mState.mSelectionStart;
	auto oldSelEnd   = mState.mSelectionEnd;

	mState.mSelectionStart = SanitizeCoordinates(aStart);
	mState.mSelectionEnd = SanitizeCoordinates(aEnd);
	
	auto checkStartChar = mState.mSelectionStart;

	if (aStart > aEnd)
		std::swap(mState.mSelectionStart, mState.mSelectionEnd);

	switch (aMode)
	{
	case TextEditor::SelectionMode::Normal:
		break;
	case TextEditor::SelectionMode::Word:
	{
		mState.mSelectionStart = FindWordStart(mState.mSelectionStart);
		if (!IsOnWordBoundary(mState.mSelectionEnd)) {
			mState.mSelectionEnd = FindWordEnd(FindWordStart(mState.mSelectionEnd));
//			if (mState.mSelectionEnd.mColumn > 0 && mState.mSelectionEnd.mColumn < mLines[mState.mSelectionEnd.mLine].size() ) {
//				auto& line = mLines[mState.mSelectionEnd.mLine];
//				if (line[mState.mSelectionEnd.mColumn].mColorIndex != line[mState.mSelectionEnd.mColumn - 1].mColorIndex)
//					mState.mSelectionEnd.mColumn--;
//			}

			if (mState.mSelectionStart != oldSelStart || mState.mSelectionEnd != oldSelEnd) {
				if (mState.mSelectionStart.mLine == mState.mSelectionEnd.mLine) {
					auto& line = mLines[checkStartChar.mLine];
					if (line[checkStartChar.mColumn].mChar != ' ' && line[checkStartChar.mColumn].mChar != '\t') {
						if (strlen(GetSelectedText().c_str()) < MAX_PATH) {
							strcpy(lastKeywordSelected, GetSelectedText().c_str());
						}
					}
				}
				mCursorPositionChanged = true;
				return;
			}
		}
		break;
	}
	case TextEditor::SelectionMode::Line:
	{
		const auto lineNo = mState.mSelectionEnd.mLine;
		const auto lineSize = (size_t)lineNo < mLines.size() ? mLines[lineNo].size() : 0;
		mState.mSelectionStart = Coordinates(mState.mSelectionStart.mLine, 0);
		mState.mSelectionEnd = Coordinates(lineNo, (int) lineSize);
		if (mState.mSelectionEnd.mColumn == mLines[lineNo].size()) {
			if (mState.mSelectionEnd.mLine < mLines.size()) {
				mState.mSelectionEnd.mLine++;
				mState.mSelectionEnd.mColumn = 0;
			}
		}
		break;
	}
	default:
		break;
	}

	if (mState.mSelectionStart != oldSelStart ||
		mState.mSelectionEnd != oldSelEnd) {
		mCursorPositionChanged = true;
		strcpy(lastKeywordSelected, "");
	}
}

void TextEditor::InsertText(const std::string & aValue)
{
	InsertText(aValue.c_str());
}

void TextEditor::InsertText(const char * aValue)
{
	if (aValue == nullptr)
		return;

	auto pos = GetActualCursorCoordinates();
	auto start = std::min(pos, mState.mSelectionStart);
	int totalLines = pos.mLine - start.mLine;

	totalLines += InsertTextAt(pos, aValue);

	SetSelection(pos, pos);

	SetCursorPosition(pos);
	//Something wrong here ?
	Colorize(start.mLine - 1, totalLines + 2);
}

void TextEditor::DeleteSelection()
{
	assert(mState.mSelectionEnd >= mState.mSelectionStart);

	if (mState.mSelectionEnd == mState.mSelectionStart)
		return;

	DeleteRange(mState.mSelectionStart, mState.mSelectionEnd);

	SetSelection(mState.mSelectionStart, mState.mSelectionStart);
	SetCursorPosition(mState.mSelectionStart);
	Colorize(mState.mSelectionStart.mLine, 1);
}

void TextEditor::MoveUp(int aAmount, bool aSelect)
{
	auto oldPos = mState.mCursorPosition;

	//PE: code folding.
	//until visible line.
	if (pref.iEnableCodeFolding && mState.mCursorPosition.mLine < MAXPRECHECKLINES) {
		int movelines = aAmount;
		aAmount = 1;
		while (movelines-- > 0) {
			if ((mState.mCursorPosition.mLine - aAmount) < MAXPRECHECKLINES) {
				while (!m_bLineVisible[mState.mCursorPosition.mLine - aAmount]) {
					if (mState.mCursorPosition.mLine - aAmount <= 0)
						break;
					aAmount++;
				}
				if (movelines > 0)
					aAmount++;

			}
		}
	}

	mState.mCursorPosition.mLine = std::max(0, mState.mCursorPosition.mLine - aAmount);
	if (oldPos != mState.mCursorPosition)
	{
		if (aSelect)
		{
			if (oldPos == mInteractiveStart)
				mInteractiveStart = mState.mCursorPosition;
			else if (oldPos == mInteractiveEnd)
				mInteractiveEnd = mState.mCursorPosition;
			else
			{
				mInteractiveStart = mState.mCursorPosition;
				mInteractiveEnd = oldPos;
			}
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);

		EnsureCursorVisible();
	}
}

void TextEditor::MoveDown(int aAmount, bool aSelect)
{
	assert(mState.mCursorPosition.mColumn >= 0);
	auto oldPos = mState.mCursorPosition;
	
	//PE: code folding.
	//until visible line.
	if (pref.iEnableCodeFolding && mState.mCursorPosition.mLine + aAmount < MAXPRECHECKLINES) {
		int movelines = aAmount;
		aAmount = 1;
		while (movelines-- > 0) {
			if ((mState.mCursorPosition.mLine + aAmount) < MAXPRECHECKLINES) {
				while (!m_bLineVisible[mState.mCursorPosition.mLine + aAmount] && (mState.mCursorPosition.mLine + aAmount) < MAXPRECHECKLINES) {
					if (mState.mCursorPosition.mLine + aAmount <= 0)
						break;
					if (mState.mCursorPosition.mLine + aAmount >= mLines.size() - 1)
						break;
					aAmount++;
				}
				if(movelines > 0)
					aAmount++;
			}
		}
	}

	mState.mCursorPosition.mLine = std::max(0, std::min((int)mLines.size() - 1, mState.mCursorPosition.mLine + aAmount));

	if (mState.mCursorPosition != oldPos)
	{
		if (aSelect)
		{
			if (oldPos == mInteractiveEnd)
				mInteractiveEnd = mState.mCursorPosition;
			else if (oldPos == mInteractiveStart)
				mInteractiveStart = mState.mCursorPosition;
			else
			{
				mInteractiveStart = oldPos;
				mInteractiveEnd = mState.mCursorPosition;
			}
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);

		EnsureCursorVisible();
	}
}

void TextEditor::MoveLeft(int aAmount, bool aSelect, bool aWordMode)
{
	if (mLines.empty())
		return;

	auto oldPos = mState.mCursorPosition;
	auto oldEnd = mInteractiveEnd;
	if (mInteractiveEnd == mInteractiveStart)
		oldEnd = oldPos;

	mState.mCursorPosition = GetActualCursorCoordinates();

	while (aAmount-- > 0)
	{
		if (mState.mCursorPosition.mColumn == 0)
		{
			if (mState.mCursorPosition.mLine > 0)
			{
				--mState.mCursorPosition.mLine;
				//PE: code folding.
				//until visible line.
				if (pref.iEnableCodeFolding) {
					if ((mState.mCursorPosition.mLine) < MAXPRECHECKLINES) {
						while (!m_bLineVisible[mState.mCursorPosition.mLine] && (mState.mCursorPosition.mLine) > 0 ) {
							if (mState.mCursorPosition.mLine <= 0)
								break;
							if (mState.mCursorPosition.mLine >= mLines.size() - 1)
								break;
							--mState.mCursorPosition.mLine;
						}
					}
				}

				mState.mCursorPosition.mColumn = (int)mLines[mState.mCursorPosition.mLine].size();
			}
		}
		else
		{
			mState.mCursorPosition.mColumn = std::max(0, mState.mCursorPosition.mColumn - 1);
			if (aWordMode)
				mState.mCursorPosition = FindWordStart(mState.mCursorPosition);
		}
	}

	assert(mState.mCursorPosition.mColumn >= 0);
	if (aSelect)
	{
		if (oldPos == mInteractiveStart) {
			mInteractiveStart = mState.mCursorPosition;
			if (mInteractiveEnd == oldPos) {
				if (mInteractiveEnd.mColumn > 0 && aWordMode) {

					mInteractiveEnd.mColumn--;
					SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode ? SelectionMode::Word : SelectionMode::Normal);
					mInteractiveEnd.mColumn++;

					mInteractiveEnd = oldEnd;
					mState.mSelectionEnd = oldEnd;

					EnsureCursorVisible();
					return;
				}
			}
			else {
//				if (oldPos == mInteractiveEnd) {
//					mInteractiveEnd = mState.mCursorPosition;
//					if (mInteractiveEnd.mColumn > 0 && aWordMode) {
//						mInteractiveEnd.mColumn--;
//						SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode ? SelectionMode::Word : SelectionMode::Normal);
//						mInteractiveEnd.mColumn++;
//						EnsureCursorVisible();
//						return;
//					}
//				}
			}
		}
		else if (oldPos == mInteractiveEnd) {
			mInteractiveEnd = mState.mCursorPosition;
			if (mInteractiveEnd.mColumn > 0 && aWordMode) {
				mInteractiveEnd.mColumn--;
				SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode ? SelectionMode::Word : SelectionMode::Normal);
				mInteractiveEnd.mColumn++;

				EnsureCursorVisible();
				return;
			}
		}
		else
		{
			mInteractiveStart = mState.mCursorPosition;
			mInteractiveEnd = oldPos;
		}
	}
	else {
		//PE: Fix - When highlight a section of code, and press the left arrow, it should put cursor at start of highlighted text  
		if (HasSelection()) {

			if (mState.mSelectionEnd != mState.mSelectionStart) {
				if (mState.mSelectionEnd < mState.mSelectionStart)
					mState.mCursorPosition = mState.mSelectionEnd;
				else
					mState.mCursorPosition = mState.mSelectionStart;
			}
		}
		mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
	}
	
	if (mInteractiveEnd > mInteractiveStart && mInteractiveEnd.mColumn > 0 && aWordMode) {
		mInteractiveEnd.mColumn--;
		SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode ? SelectionMode::Word : SelectionMode::Normal);
		mInteractiveEnd.mColumn++;

		mInteractiveEnd = oldEnd;
		mState.mSelectionEnd = oldEnd;

		EnsureCursorVisible();
		return;
	}
	else {
		SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode ? SelectionMode::Word : SelectionMode::Normal);

	}

	EnsureCursorVisible();
}

void TextEditor::MoveRight(int aAmount, bool aSelect, bool aWordMode)
{


	auto oldPos = mState.mCursorPosition;
	auto oldStart = mInteractiveStart;
	if (mInteractiveEnd == mInteractiveStart)
		oldStart = oldPos;

	if (HasSelection() && aSelect && selectedfrommouse ) {
		mState.mCursorPosition = mState.mSelectionEnd;
		oldPos = mInteractiveEnd;
		mInteractiveStart = mState.mSelectionStart;
	}


	if (mLines.empty())
		return;

	while (aAmount-- > 0)
	{
		auto& line = mLines[mState.mCursorPosition.mLine];
		if (mState.mCursorPosition.mColumn >= (int)line.size())
		{
			if (mState.mCursorPosition.mLine < (int)mLines.size() - 1)
			{
				mState.mCursorPosition.mLine = std::max(0, std::min((int)mLines.size() - 1, mState.mCursorPosition.mLine + 1));
				//PE: code folding.
				//until visible line.
				if (pref.iEnableCodeFolding) {
					if ((mState.mCursorPosition.mLine) < MAXPRECHECKLINES) {
						while (!m_bLineVisible[mState.mCursorPosition.mLine] && mState.mCursorPosition.mLine < MAXPRECHECKLINES ) {
							if (mState.mCursorPosition.mLine <= 0)
								break;
							if (mState.mCursorPosition.mLine >= mLines.size() - 1)
								break;
							++mState.mCursorPosition.mLine;
						}
					}
				}

				mState.mCursorPosition.mColumn = 0;
			}
		}
		else
		{
			mState.mCursorPosition.mColumn = std::max(0, std::min((int)line.size(), mState.mCursorPosition.mColumn + 1));
			if (aWordMode)
				mState.mCursorPosition = FindWordEnd(mState.mCursorPosition);
		}
	}

	if (aSelect)
	{
		if (oldPos == mInteractiveEnd) {
			//mInteractiveEnd = mState.mCursorPosition;
			//mInteractiveEnd.mColumn --; //Mouse start at 1 and selection at 0
			//mInteractiveEnd = SanitizeCoordinates(mInteractiveEnd);
			mInteractiveEnd = SanitizeCoordinates(mState.mCursorPosition);
			if (mInteractiveEnd.mColumn > 0 && aWordMode) {
				mInteractiveEnd.mColumn--;
				SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode ? SelectionMode::Word : SelectionMode::Normal);
				mInteractiveEnd.mColumn++;

				mInteractiveStart = oldStart;
				mState.mSelectionStart = oldStart;

				EnsureCursorVisible();
				return;
			}
		}
		else if (oldPos == mInteractiveStart)
			mInteractiveStart = mState.mCursorPosition;
		else
		{
			mInteractiveStart = oldPos;
//			mInteractiveEnd = mState.mCursorPosition;
			mInteractiveEnd = SanitizeCoordinates(mState.mCursorPosition);
			if (mInteractiveEnd.mColumn > 0 && aWordMode) {
				mInteractiveEnd.mColumn--;
				SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode ? SelectionMode::Word : SelectionMode::Normal);
				mInteractiveEnd.mColumn++;

				mInteractiveStart = oldStart;
				mState.mSelectionStart = oldStart;

				EnsureCursorVisible();
				return;
			}

		}
	}
	else {
		//PE: Fix - When highlight a section of code, and press the right arrow, it should put cursor at end of highlighted text  
		if (HasSelection()) {
			if (mState.mSelectionEnd != mState.mSelectionStart) {
				if (mState.mSelectionEnd < mState.mSelectionStart)
					mState.mCursorPosition = mState.mSelectionStart;
				else
					mState.mCursorPosition = mState.mSelectionEnd;
			}
		}

		mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
	}
	SetSelection(mInteractiveStart, mInteractiveEnd, aSelect && aWordMode ? SelectionMode::Word : SelectionMode::Normal);

	EnsureCursorVisible();
}

void TextEditor::MoveTop(bool aSelect)
{
	auto oldPos = mState.mCursorPosition;
	SetCursorPosition(Coordinates(0, 0));

	if (mState.mCursorPosition != oldPos)
	{
		if (aSelect)
		{
			mInteractiveEnd = oldPos;
			mInteractiveStart = mState.mCursorPosition;
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);
	}
}

void TextEditor::TextEditor::MoveBottom(bool aSelect)
{
	auto oldPos = GetCursorPosition();
	int newCol = (int) mLines[mLines.size() - 1].size();
	if (newCol < 0) newCol = 0;
	auto newPos = Coordinates((int)mLines.size() - 1, newCol);
	SetCursorPosition(newPos);
	if (aSelect)
	{
		mInteractiveStart = oldPos;
		mInteractiveEnd = newPos;
	}
	else
		mInteractiveStart = mInteractiveEnd = newPos;
	SetSelection(mInteractiveStart, mInteractiveEnd);
}

bool bTobbleCtrlHome = true;

void TextEditor::MoveHome(bool aSelect)
{
	auto oldPos = mState.mCursorPosition;
	SetCursorPosition(Coordinates(mState.mCursorPosition.mLine, 0));

	if (mInteractiveStart == mInteractiveEnd)
		bTobbleCtrlHome = true;

	if (mState.mCursorPosition != oldPos || (aSelect && bTobbleCtrlHome) )
	{
		if (aSelect)
		{
			//Toggle select all , select without
			if (oldPos == mInteractiveStart) {
				//toggle ctrl home
				mInteractiveStart = mState.mCursorPosition;
				if(bTobbleCtrlHome)
				{
					auto& line = mLines[mState.mCursorPosition.mLine];
					//\t first char dont select. start = 1
					for (size_t it = 0; it < line.size() && isblank((unsigned char)line[it].mChar); ++it)
						mInteractiveStart.mColumn++;
					if(oldPos == mInteractiveStart || mInteractiveStart.mColumn > mInteractiveEnd.mColumn )
						mInteractiveStart = mState.mCursorPosition;
					else
						SetCursorPosition(Coordinates(mState.mCursorPosition.mLine, mInteractiveStart.mColumn));
					bTobbleCtrlHome = false;
				}
				else {
					bTobbleCtrlHome = true;
				}

			}
			else if (oldPos == mInteractiveEnd)
				mInteractiveEnd = mState.mCursorPosition;
			else
			{
				mInteractiveStart = mState.mCursorPosition;
				mInteractiveEnd = oldPos;
			}
		}
		else {
			auto& line = mLines[mState.mCursorPosition.mLine];
			//\t first char dont select. start = 1
			//mState.mCursorPosition.mColumn = oldPos.mColumn;
			mState.mCursorPosition.mColumn = 0;
			for (size_t it = 0; it < line.size() && isblank((unsigned char)line[it].mChar); ++it)
				mState.mCursorPosition.mColumn++;
			if (mState.mCursorPosition.mColumn == oldPos.mColumn)
				mState.mCursorPosition.mColumn = 0;
			SetCursorPosition(Coordinates(mState.mCursorPosition.mLine, mState.mCursorPosition.mColumn));
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		}
		SetSelection(mInteractiveStart, mInteractiveEnd);
	}
	else {
		if (!aSelect) {
			auto& line = mLines[mState.mCursorPosition.mLine];
			mState.mCursorPosition.mColumn = oldPos.mColumn;
			for (size_t it = 0; it < line.size() && isblank((unsigned char)line[it].mChar); ++it)
				mState.mCursorPosition.mColumn++;
			if (mState.mCursorPosition.mColumn == oldPos.mColumn)
				mState.mCursorPosition.mColumn = 0;
			SetCursorPosition(Coordinates(mState.mCursorPosition.mLine, mState.mCursorPosition.mColumn));

			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
			SetSelection(mInteractiveStart, mInteractiveEnd);
		}
	}
}

void TextEditor::MoveEnd(bool aSelect)
{
	auto oldPos = mState.mCursorPosition;
	SetCursorPosition(Coordinates(mState.mCursorPosition.mLine, (int)mLines[oldPos.mLine].size()));

	if (mState.mCursorPosition != oldPos)
	{
		if (aSelect)
		{
			if (oldPos == mInteractiveEnd)
				mInteractiveEnd = mState.mCursorPosition;
			else if (oldPos == mInteractiveStart)
				mInteractiveStart = mState.mCursorPosition;
			else
			{
				mInteractiveStart = oldPos;
				mInteractiveEnd = mState.mCursorPosition;
			}
		}
		else
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
		SetSelection(mInteractiveStart, mInteractiveEnd);
	}
	else {
		if (!aSelect) {
			mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;
			SetSelection(mInteractiveStart, mInteractiveEnd);
		}
	}
}

void TextEditor::Delete()
{
	assert(!mReadOnly);

	if (mLines.empty())
		return;
	filechanged = true;
	UndoRecord u;
	u.mBefore = mState;

	if (HasSelection())
	{
		u.mRemoved = GetSelectedText();
		u.mRemovedStart = mState.mSelectionStart;
		u.mRemovedEnd = mState.mSelectionEnd;

		DeleteSelection();
	}
	else
	{
		auto pos = GetActualCursorCoordinates();
		SetCursorPosition(pos);
		auto& line = mLines[pos.mLine];

		if (pos.mColumn == (int)line.size())
		{
			if (pos.mLine == (int)mLines.size() - 1)
				return;

			u.mRemoved = '\n';
			u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
			Advance(u.mRemovedEnd);

			auto& nextLine = mLines[pos.mLine + 1];
			line.insert(line.end(), nextLine.begin(), nextLine.end());
			RemoveLine(pos.mLine + 1);
		}
		else
		{
			//UTF8: need support here.
			if (line[pos.mColumn].mInt > 0) {
				std::string ms = UnicodeToUTF8(line[pos.mColumn].mInt);
				for (int a = 0; a < ms.size(); a++) {
					u.mRemoved.push_back(ms[a]);
				}
				u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
				u.mRemovedEnd.mColumn++;
			}
			else {
				u.mRemoved = line[pos.mColumn].mChar;
				u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
				u.mRemovedEnd.mColumn++;
			}
			line.erase(line.begin() + pos.mColumn);
		}

		mTextChanged = true;

		Colorize(pos.mLine, 1);
	}

	u.mAfter = mState;
	AddUndo(u);
}

void TextEditor::BackSpace()
{
	assert(!mReadOnly);

	if (mLines.empty())
		return;

	UndoRecord u;
	u.mBefore = mState;
	
	filechanged = true;

	if (HasSelection())
	{
		u.mRemoved = GetSelectedText();
		u.mRemovedStart = mState.mSelectionStart;
		u.mRemovedEnd = mState.mSelectionEnd;

		DeleteSelection();

		mState.mSelectionStart = mState.mSelectionEnd = mState.mCursorPosition;
		mInteractiveStart = mInteractiveEnd = mState.mCursorPosition;

	}
	else
	{
		auto pos = GetActualCursorCoordinates();
		SetCursorPosition(pos);

		if (mState.mCursorPosition.mColumn == 0)
		{
			if (mState.mCursorPosition.mLine == 0)
				return;

			u.mRemoved = '\n';
			u.mRemovedStart = u.mRemovedEnd = Coordinates(pos.mLine - 1, (int)mLines[pos.mLine - 1].size());
			Advance(u.mRemovedEnd);

			auto& line = mLines[mState.mCursorPosition.mLine];
			auto& prevLine = mLines[mState.mCursorPosition.mLine - 1];
			auto prevSize = (int)prevLine.size();
			prevLine.insert(prevLine.end(), line.begin(), line.end());

			ErrorMarkers etmp;
			for (auto& i : mErrorMarkers)
				etmp.insert(ErrorMarkers::value_type(i.first - 1 == mState.mCursorPosition.mLine ? i.first - 1 : i.first, i.second));
			mErrorMarkers = std::move(etmp);

			RemoveLine(mState.mCursorPosition.mLine);
			--mState.mCursorPosition.mLine;
			mState.mCursorPosition.mColumn = prevSize;
		}
		else
		{
			auto& line = mLines[mState.mCursorPosition.mLine];

			if (line[pos.mColumn - 1].mInt > 0) {
				std::string ms = UnicodeToUTF8(line[pos.mColumn - 1].mInt);
				for (int a = 0; a < ms.size(); a++) {
					u.mRemoved.push_back(ms[a]);
				}
			}
			else {
				u.mRemoved = line[pos.mColumn - 1].mChar;
			}
			
			u.mRemovedStart = u.mRemovedEnd = GetActualCursorCoordinates();
			--u.mRemovedStart.mColumn;

			--mState.mCursorPosition.mColumn;
			if (mState.mCursorPosition.mColumn < (int)line.size())
				line.erase(line.begin() + mState.mCursorPosition.mColumn);

			if (mState.mCursorPosition.mColumn > 0) {
				line = mLines[mState.mCursorPosition.mLine];
				int a, b, c;
				for (a = mState.mCursorPosition.mColumn - 1; a > 0; a--) {
					if (line[a].mChar == ' ' || line[a].mChar == '\t')
						break;
				}
				c = 0;
				for (b = a; b < mState.mCursorPosition.mColumn; b++) {
					lastKeyedWord[c++] = line[b].mChar;
					lastKeyedWord[c] = 0;
					if (c >= MAX_PATH-1)
						break;
				}
			}
			else
				lastKeyedWord[0] = 0;
		}

		mTextChanged = true;

		EnsureCursorVisible();
		Colorize(mState.mCursorPosition.mLine, 1);
	}

	u.mAfter = mState;
	AddUndo(u);
}

void TextEditor::SelectWordUnderCursor()
{
	auto c = GetCursorPosition();
	SetSelection(FindWordStart(c), FindWordEnd(c));
}

void TextEditor::SelectAll()
{
	SetSelection(Coordinates(0, 0), Coordinates((int)mLines.size(), 0));
}

bool TextEditor::HasSelection() const
{
	return mState.mSelectionEnd > mState.mSelectionStart;
}

void TextEditor::Copy()
{

	if (HasSelection())
	{
		//UTF8: Not needed anymore gettext do this.
		//std::string buffer,gst;
		//gst = GetSelectedText();

//This is not a general rule.
//		const auto& line = mLines[mState.mSelectionEnd.mLine];
//		if (mState.mSelectionEnd.mColumn == (int)line.size()) {
//			//Add newline.
//			gst.append("\n");
//		}


		//UTF8: Not needed anymore gettext do this.
/*
		unsigned char *s = (unsigned char *) gst.c_str();
		for (int i = 0; i < gst.length(); i++) {
			char utf8[4];
			unsigned int c = (unsigned char)*s;
			if (CharToUtf8(&utf8[0], 2, c) == 2)
			{
				buffer.push_back(utf8[0]);
				buffer.push_back(utf8[1]);
			}
			else {
				if (*s == '\n') {
					buffer.push_back('\r');
				}
				buffer.push_back(*s);
			}
			s++;
		}
		ImGui::SetClipboardText(buffer.c_str());
*/
		ImGui::SetClipboardText(GetSelectedText().c_str());

	}
	else
	{
		if (!mLines.empty())
		{
			std::string str;
			auto& line = mLines[GetActualCursorCoordinates().mLine];
			char utf8[4];
			for (auto& g : line) {
				//UTF8: need support here.
				unsigned int c = (unsigned char)g.mChar;

				if (g.mInt > 0) {
					std::string ms = UnicodeToUTF8(g.mInt);
					for (int a = 0; a < ms.size(); a++) {
						str.push_back(ms[a]);
					}
				}
				else {

					if (CharToUtf8(&utf8[0], 2, c) == 2)
					{
						str.push_back(utf8[0]);
						str.push_back(utf8[1]);
					}
					else {
						str.push_back(g.mChar);
					}
				}
			}
			ImGui::SetClipboardText(str.c_str());
		}
	}
}

void TextEditor::Cut()
{
	if (IsReadOnly())
	{
		Copy();
	}
	else
	{
		filechanged = true;
		if (HasSelection())
		{
			UndoRecord u;
			u.mBefore = mState;
			u.mRemoved = GetSelectedText();
			u.mRemovedStart = mState.mSelectionStart;
			u.mRemovedEnd = mState.mSelectionEnd;

			Copy();
			DeleteSelection();

			u.mAfter = mState;
			AddUndo(u);
		}
	}
}

void TextEditor::InsertTextDirectly(const char *text)
{
	auto clipText = ImGui::GetClipboardText();
	clipText = text;
	if (text != nullptr && strlen(text) > 0)
	{
		filechanged = true;
		UndoRecord u;
		u.mBefore = mState;

		u.mAdded = text;
		u.mAddedStart = GetActualCursorCoordinates();

		InsertText(clipText);

		u.mAddedEnd = GetActualCursorCoordinates();
		u.mAfter = mState;
		AddUndo(u);
	}
}


//Direct replace no undo.
void TextEditor::DirectReplace(char * string, char * replace,int lineno)
{
	if (string == NULL || replace == NULL)
		return;
	if (mLines.empty())
		return;

	auto oldmouse = mState.mCursorPosition;
	auto oldselectionstart = mState.mSelectionStart;
	auto oldselectionend = mState.mSelectionEnd;

	auto& line = mLines[lineno-1];

	if (line.empty())
		return;

	char * buffer = new char[line.size() + 1];
	for (size_t j = 0; j < line.size(); ++j)
	{
		auto& col = line[j];
		buffer[j] = col.mChar;
		buffer[j + 1] = 0;
	}
	char *found;
	found = (char*)pestrcasestr(&buffer[0], string);
	if (found)
	{
		int pos = found - &buffer[0];
		if (pos > 0 && (found[-1] != ' ' && found[-1] != '\t'))
			found = 0;
		delete[] buffer;
		if (found) {
			SetCursorPosition(TextEditor::Coordinates(lineno - 1, pos));
			mState.mSelectionStart = TextEditor::Coordinates(lineno - 1, pos);
			mState.mSelectionEnd = TextEditor::Coordinates(lineno - 1, pos + strlen(string));
			if (mState.mSelectionEnd > mState.mSelectionStart) {
				DeleteRange(mState.mSelectionStart, mState.mSelectionEnd);
				mState.mSelectionEnd = mState.mSelectionStart;
				//SetSelection(mState.mSelectionStart, mState.mSelectionStart);
				mState.mCursorPosition = mState.mSelectionStart;
				InsertTextAt(mState.mCursorPosition, replace);
				Colorize(mState.mCursorPosition.mLine - 1, mState.mCursorPosition.mLine + 1);
				//InsertText(replace);
				mScrollToCursor = false;
				mTextChanged = false;
				filechanged = true;
			}
			mState.mSelectionStart = oldselectionstart;
			mState.mSelectionEnd = oldselectionend;
			mState.mCursorPosition = oldmouse;
		}
		return;
	}
	delete[] buffer;
}

void TextEditor::PasteString(char * string)
{
	if (string == NULL)
		return;
	auto clipText = string;
	//UTF8 to unicode point.
//	UTF8ToUniCodePoint((char *)clipText);
//	UTF8ToUniCodePoint((char *)clipText);
//	UTF8ToUniCodePoint((char *)clipText);


	if (clipText != nullptr && strlen(clipText) > 0)
	{
		filechanged = true;
		UndoRecord u;
		u.mBefore = mState;

		if (HasSelection())
		{
			u.mRemoved = GetSelectedText();
			u.mRemovedStart = mState.mSelectionStart;
			u.mRemovedEnd = mState.mSelectionEnd;
			DeleteSelection();
		}

		u.mAdded = clipText;
		u.mAddedStart = GetActualCursorCoordinates();

		InsertText(clipText);

		u.mAddedEnd = GetActualCursorCoordinates();
		u.mAfter = mState;
		AddUndo(u);
	}
}


void TextEditor::Paste()
{
	auto clipText = ImGui::GetClipboardText();

	if (!clipText)
		return;

	//UTF8 to unicode point.
	//UTF8: Keep in utf8 format.
	//inserttext will do the convert.
	//UTF8ToUniCodePoint((char *) clipText);
	//UTF8ToUniCodePoint((char *)clipText);
	//UTF8ToUniCodePoint((char *)clipText);


	if (clipText != nullptr && strlen(clipText) > 0)
	{
		filechanged = true;
		UndoRecord u;
		u.mBefore = mState;

		if (HasSelection())
		{
			u.mRemoved = GetSelectedText();
			u.mRemovedStart = mState.mSelectionStart;
			u.mRemovedEnd = mState.mSelectionEnd;
			DeleteSelection();
		}

		u.mAdded = clipText;
		u.mAddedStart = GetActualCursorCoordinates();

		InsertText(clipText);

		u.mAddedEnd = GetActualCursorCoordinates();
		u.mAfter = mState;
		AddUndo(u);
	}
}

bool TextEditor::CanUndo() const
{
	return mUndoIndex > 0;
}

bool TextEditor::CanRedo() const
{
	return mUndoIndex < (int)mUndoBuffer.size();
}

void TextEditor::Undo(int aSteps)
{
	while (CanUndo() && aSteps-- > 0) {
		filechanged = true;
		mUndoBuffer[--mUndoIndex].Undo(this);
	}
}

void TextEditor::Redo(int aSteps)
{
	while (CanRedo() && aSteps-- > 0) {
		filechanged = true;
		mUndoBuffer[mUndoIndex++].Redo(this);
	}
}

const TextEditor::Palette & TextEditor::GetCustomPalette()
{
	pref.editorPalette = 4;
	return pref.EditorColors;
}

//PE: Format : Alpha 0x00,Blue 0x00,Green 0x00,Red 0x00
const TextEditor::Palette & TextEditor::GetAGKPalette()
{
	pref.editorPalette = 0;
	static Palette p = { {
			0xffcccccc,	// None
			0xffd79c56,	// Keyword, do loop while for next 	0xffb2c856
			0xff00ff00,	// Number
			0xff4778ff,	// String //0xff7070e0
			0xff70a0e0, // Char literal
			0xffffffff, // Punctuation
			0xff409090,	// Preprocessor
			0xffcccccc, // Identifier
			0xffb2c856, // Known identifier, agk commands
			0xffc040a0, // Preproc identifier
			0xff4aaa56, // Comment (single line) 0xff206020
			0xff4aaa56, // Comment (multi line) 0xff406020
			0xff181818, // Background
			0xffe0e0e0, // Cursor
			0x80808080, // Selection 0x80a06020.
			0x800020ff, // ErrorMarker
			0x40f08000, // Breakpoint
			0xff808080, // Line number
			0x70b0b0b0, // last keyword mark.
			0x40808080, // Current line fill (inactive)
			0x40a0a0a0, // Current line edge
			0xff1e1e1e, // Background Line
		} };
	return p;
}

const TextEditor::Palette & TextEditor::GetDarkPalette()
{
	pref.editorPalette = 1;
	static Palette p = { {
		0xffaaaaaa,	// None
		0xffd69c56,	// Keyword	
		0xff00ff00,	// Number
		0xff7070e0,	// String
		0xff70a0e0, // Char literal
		0xffffffff, // Punctuation
		0xff409090,	// Preprocessor
		0xffaaaaaa, // Identifier
		0xff9bc64d, // Known identifier
		0xffc040a0, // Preproc identifier
		0xff208020, // Comment (single line) 0xff206020
		0xff408020, // Comment (multi line) 0xff406020
		0xff101010, // Background
		0xffe0e0e0, // Cursor
		0x80808080, // Selection 0x80a06020.
		0x800020ff, // ErrorMarker
		0x40f08000, // Breakpoint
		0xff707000, // Line number
		0x70b0b0b0, // last keyword mark.
		0x40808080, // Current line fill (inactive)
		0x40a0a0a0, // Current line edge
		0xff181818, // Background
	} };
	return p;
}

const TextEditor::Palette & TextEditor::GetLightPalette()
{
	pref.editorPalette = 2;
	static Palette p = { {
		0xff202020,	// None 0xff000000
		0xffff0c06,	// Keyword	
		0xff008000,	// Number
		0xff2020a0,	// String
		0xff304070, // Char literal
		0xff000000, // Punctuation
		0xff409090,	// Preprocessor
		0xff202020, // Identifier
		0xff606010, // Known identifier
		0xffc040a0, // Preproc identifier
		0xff707070, // Comment (single line) 0xff205020
		0xff707070, // Comment (multi line) 0xff405020
		0xffffffff, // Background
		0xff000000, // Cursor
		0x70600000, // Selection 0x80600000
		0xa00010ff, // ErrorMarker
		0x80f08000, // Breakpoint
		0xff505000, // Line number
		0x70a00000, // Current line fill
		0x40808080, // Current line fill (inactive)
		0x40000000, // Current line edge
		0xffefefef, // Background
	} };
	return p;
}

const TextEditor::Palette & TextEditor::GetRetroBluePalette()
{
	pref.editorPalette = 3;
	static Palette p = { {
		0xff00ffff,	// None
		0xffffff00,	// Keyword	
		0xff00ff00,	// Number
		0xff808000,	// String
		0xff808000, // Char literal
		0xffffffff, // Punctuation
		0xff008000,	// Preprocessor
		0xff00ffff, // Identifier
		0xffffffff, // Known identifier
		0xffff00ff, // Preproc identifier
		0xff808080, // Comment (single line)
		0xff404040, // Comment (multi line)
		0xff800000, // Background
		0xff0080ff, // Cursor
		0x80ffff00, // Selection
		0xa00000ff, // ErrorMarker
		0x80ff8000, // Breakpoint
		0xff808000, // Line number
		0x40000000, // Current line fill
		0x40808080, // Current line fill (inactive)
		0x40000000, // Current line edge
	} };
	return p;
}


std::string TextEditor::GetText() const
{
	return GetText(Coordinates(), Coordinates((int)mLines.size(), 0));
}

std::vector<std::string> TextEditor::GetTextLines() const
{
	std::vector<std::string> result;
	
	result.reserve(mLines.size());
	
	for (auto & line : mLines)
	{
		std::string text;
		
		text.resize(line.size());
		
		for (size_t i = 0; i < line.size(); ++i)
			text[i] = line[i].mChar;
		
		result.emplace_back(std::move(text));
	}
	
	return result;
}

std::string TextEditor::GetSelectedText() const
{
	return GetText(mState.mSelectionStart, mState.mSelectionEnd);
}

std::string TextEditor::GetCurrentLineText()const
{
	auto lineLength = (int) mLines[mState.mCursorPosition.mLine].size();
	return GetText(Coordinates(mState.mCursorPosition.mLine, 0), Coordinates(mState.mCursorPosition.mLine, lineLength));
}

void TextEditor::ProcessInputs()
{
}

void TextEditor::Colorize(int aFromLine, int aLines)
{
	int toLine = aLines == -1 ? (int)mLines.size() : std::min((int)mLines.size(), aFromLine + aLines);
	mColorRangeMin = std::min(mColorRangeMin, aFromLine);
	mColorRangeMax = std::max(mColorRangeMax, toLine);
	mColorRangeMin = std::max(0, mColorRangeMin);
	mColorRangeMax = std::max(mColorRangeMin, mColorRangeMax);
	mCheckMultilineComments = true;
}

void TextEditor::ColorizeRange(int aFromLine, int aToLine)
{
	if (aFromLine >= aToLine || mLines.empty())
		return;

	std::string buffer;
	std::cmatch results;
	std::string id;
	
	int endLine = std::max(0, std::min((int)mLines.size(), aToLine));
	for (int i = aFromLine; i < endLine; ++i)
	{
		bool preproc = false;
		auto& line = mLines[i];
		
		if( i < MAXPRECHECKLINES )
			m_bFirstCheck[i] = true;

		if (line.empty()) {
			if (i < MAXPRECHECKLINES) {
				m_bCheckLine[i] = false;
				if (pref.iEnableCodeFolding) {
					m_bFoldingStartWord[i] = false;
					m_bFoldingEndWord[i] = false;
				}
			}
			continue;
		}

		buffer.resize(line.size());
		for (size_t j = 0; j < line.size(); ++j)
		{
			auto& col = line[j];
			buffer[j] = col.mChar;
			//buffer[j + 1] = 0;
			col.mColorIndex = PaletteIndex::Default;
		}

		const char * bufferBegin = &buffer.front();
		const char * bufferEnd = bufferBegin + buffer.size();
		
		auto last = bufferEnd;
		

		for (auto first = bufferBegin; first != last; )
		{
			const char * token_begin = nullptr;
			const char * token_end = nullptr;
			PaletteIndex token_color = PaletteIndex::Default;
			
			bool hasTokenizeResult = false;
		
			if (mLanguageDefinition.mTokenize != nullptr)
			{
				if (mLanguageDefinition.mTokenize(first, last, token_begin, token_end, token_color))
					hasTokenizeResult = true;
			}
			
			if (first == bufferBegin) {
				char *tmp = (char *) first;
				//Only check on begin.
				if (last - first >= 2 && ( first[0] == '\'' || first[0] == '`') && (first[1] == ' ' || first[1] == '\t' ) ) {
					hasTokenizeResult = true;
					token_begin = first;
					token_end = last;
					token_color = PaletteIndex::Comment;
				}

				while (tmp[0] != 0 && (tmp[0] == ' ' || tmp[0] == '\t'))
					tmp++;
				char *start = tmp;
				while (tmp[0] != 0 && strchr(ISALETTERDIGIT, tmp[0]))
					tmp++;
				if (tmp[0] != 0 && tmp[0] == ':' && tmp > first) {
					//Gosub block.
					hasTokenizeResult = true;
					token_begin = first;
					token_end = tmp;
					token_color = PaletteIndex::Preprocessor;
					// PreprocIdentifier - lilla
					//KnownIdentifier cyan.
					//CharLiteral light red
					//Punctuation = white
					//Preprocessor = dark yellow
				}
			}

			if (hasTokenizeResult == false)
			{
				// todo : remove
				//printf("using regex for %.*s\n", first + 10 < last ? 10 : int(last - first), first);
				
				for (auto& p : mRegexList)
				{

					if (isAGKsource) {
						if (p.second == PaletteIndex::Comment) {
							//PE: Faster then regex

							if (first[0] == '`') {
								hasTokenizeResult = true;
								token_begin = first;
								token_end = last;
								token_color = PaletteIndex::Comment;
								break;
							}
							if (last - first >= 2 && first[0] == '/' && first[1] == '/') {

								if (pestrcasestr(bufferBegin, "remstart") != NULL && pestrcasestr(bufferBegin, "remend") != NULL) {
									//ignore.
								}
								else {
									//PaletteIndex::Comment
									hasTokenizeResult = true;
									token_begin = first;
									token_end = last;
									token_color = PaletteIndex::Comment;
								}
								break;
							}
							//PE: 09-02-2019 , change 4 to 3 and added first + 3 == last
							if (last - first >= 3 && (first[0] == 'r' || first[0] == 'R') && (first[1] == 'e' || first[1] == 'E') && (first[2] == 'm' || first[2] == 'M') && (first + 3 == last || first[3] == ' ' || first[3] == '\t')) {
								hasTokenizeResult = true;
								token_begin = first;
								token_end = last;
								token_color = PaletteIndex::Comment;
								break;
							}
							continue;
						}

						if (p.second == PaletteIndex::CharLiteral) {
							if (last - first >= 2 && first > bufferBegin && first[0] == '\'') {
								const char *found = strstr(first + 1, "'");
								if (found != NULL) {
									hasTokenizeResult = true;
									token_begin = first;
									token_end = found + 1;
									token_color = PaletteIndex::CharLiteral;
									break;
								}
							}
							continue;
						}

						if (p.second == PaletteIndex::String) {
							if (last - first >= 2 && first[0] == '\"') {
								const char *found = strstr(first + 1, "\"");
								if (found != NULL) {
									hasTokenizeResult = true;
									token_begin = first;
									token_end = found + 1;
									token_color = PaletteIndex::String;
									break;
								}
								else {
									//PE: 26-02-2019 Mark rest of line as string
									hasTokenizeResult = true;
									token_begin = first;
									token_end = last;
									token_color = PaletteIndex::String;
									break;
								}
							}
							continue;
						}

						if (p.second == PaletteIndex::Number) {

							if (TokenizeCStyleNumber(first, last, token_begin, token_end)) {
								hasTokenizeResult = true;
								token_color = PaletteIndex::Number;
								break;
							}
							continue;
						}

						if (p.second == PaletteIndex::Identifier) {
							//TokenizeAGKStyleIdentifier
							//if (TokenizeCStyleIdentifier(first, last, token_begin, token_end)) {
							if (TokenizeAGKStyleIdentifier(first, last, token_begin, token_end)) {
								hasTokenizeResult = true;
								token_color = PaletteIndex::Identifier;
								break;
							}
							continue;
						}

					}
					if (std::regex_search(first, last, results, p.first, std::regex_constants::match_continuous))
					{
						hasTokenizeResult = true;
						auto& v = *results.begin();
						token_begin = v.first;
						token_end = v.second;
						token_color = p.second;
						break;
					}

				}
			}
			
			if (hasTokenizeResult == false)
			{
				first++;
			}
			else
			{
				const size_t token_length = token_end - token_begin;
				
				if (token_color == PaletteIndex::Identifier)
				{
					id.assign(token_begin, token_end);
					
					// todo : allmost all language definitions use lower case to specify keywords, so shouldn't this use ::tolower ?
					if (!mLanguageDefinition.mCaseSensitive)
						std::transform(id.begin(), id.end(), id.begin(), ::tolower);
					//std::transform(id.begin(), id.end(), id.begin(), ::toupper);

					if (!preproc)
					{
						if (mLanguageDefinition.mKeywords.count(id) != 0)
							token_color = PaletteIndex::Keyword;
						else if (mLanguageDefinition.mIdentifiers.count( id) != 0)
							token_color = PaletteIndex::KnownIdentifier;
						else if (mLanguageDefinition.mPreprocIdentifiers.count(id) != 0)
							token_color = PaletteIndex::PreprocIdentifier;
					}
					else
					{
						if (mLanguageDefinition.mPreprocIdentifiers.count(id) != 0)
							token_color = PaletteIndex::PreprocIdentifier;
						else
							token_color = PaletteIndex::Identifier;
					}
				}
				else if (token_color == PaletteIndex::Preprocessor)
				{
					preproc = true;
				}
				
				for (size_t j = 0; j < token_length; ++j)
					line[(token_begin - bufferBegin) + j].mColorIndex = token_color;
				
				if (token_end <= first)
					first++;
				else
					first = token_end;
			}
		}

		if (i < MAXPRECHECKLINES-4) {
			m_bCheckLine[i] = false;
			char *commentstart = (char *)pestrcasestr(bufferBegin, "remstart");
			if (!commentstart) commentstart = (char *) strstr(bufferBegin, "/*");
			if ( commentstart)
			{
				if (strstr(bufferBegin, "//*") == NULL) { // Ignore //*
					if (pestrcasestr(bufferBegin, "remend") == NULL) { // Ignore remend on same line.

						int pos = commentstart - bufferBegin;
						if (pos > 0 && pos < line.size()) {
							if (line[pos - 1].mColorIndex != PaletteIndex::Comment) {
								if (i > 0) m_bCheckLine[i - 1] = true;
								if (i > 1) m_bCheckLine[i - 2] = true;
								if (i > 2) m_bCheckLine[i - 3] = true;
								m_bCheckLine[i] = true;
								if (i < MAXPRECHECKLINES - 1) m_bCheckLine[i + 1] = true;
								if (i < MAXPRECHECKLINES - 2) m_bCheckLine[i + 2] = true;
								if (i < MAXPRECHECKLINES - 3) m_bCheckLine[i + 3] = true;
							}
						}
						else {
							if (i > 0) m_bCheckLine[i - 1] = true;
							if (i > 1) m_bCheckLine[i - 2] = true;
							if (i > 2) m_bCheckLine[i - 3] = true;
							m_bCheckLine[i] = true;
							if (i < MAXPRECHECKLINES - 1) m_bCheckLine[i + 1] = true;
							if (i < MAXPRECHECKLINES - 2) m_bCheckLine[i + 2] = true;
							if (i < MAXPRECHECKLINES - 3) m_bCheckLine[i + 3] = true;
						}
					}
				}
			}

			commentstart = (char *)pestrcasestr(bufferBegin, "remend");
			if (!commentstart) commentstart = (char *)strstr(bufferBegin, "*/");
			if (commentstart) {
				int pos = commentstart - bufferBegin;
				if (pos > 0 && pos < line.size()) {
					if (line[pos - 1].mColorIndex != PaletteIndex::Comment)
						m_bCheckLine[i] = true;
				}
				else
					m_bCheckLine[i] = true;
			}


			//PE: code folding, only find start and end words here.
			if (pref.iEnableCodeFolding) {

				m_bFoldingStartWord[i] = false;
				m_bFoldingEndWord[i] = false;

				char *found = NULL; // = (char*)pestrcasestr(bufferBegin, "FoldStart");
				if ( (found = (char*)pestrcasestr(bufferBegin, "FoldStart")) ) { // && !(char*)pestrcasestr(bufferBegin, "FoldEnd")
					//line
					int wordlen = 9;
					int pos = found - bufferBegin;
					if (line.size() == wordlen ) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingStartWord[i] = true;
						}
					}
					else if (pos >= 0 && pos+ wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingStartWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingStartWord[i] = true;
								}
							}
						}
					}

					//m_bFoldingPossible[i] = true;
				}
				if ( (found = (char*)pestrcasestr(bufferBegin, "FoldEnd")) ) { //&& !pestrcasestr(bufferBegin, "FoldStart")
					int wordlen = 7;
					int pos = found - bufferBegin;
					if (line.size() == wordlen ) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingEndWord[i] = true;
						}
					}
					else if (pos >= 0 && pos+ wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingEndWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingEndWord[i] = true;
								}
							}
						}
					}
				}
				//WHILE - ENDWHILE
				if ((found = (char*)pestrcasestr(bufferBegin, "endwhile"))) {
					int wordlen = 8;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingEndWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingEndWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingEndWord[i] = true;
								}
							}
						}
					}
				}
				if ((found = (char*)pestrcasestr(bufferBegin, "while")) ) {
					//line
					int wordlen = 5;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingStartWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingStartWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingStartWord[i] = true;
								}
							}
						}
					}
				}
				//DO - LOOP
				if ((found = (char*)pestrcasestr(bufferBegin, "do")) ) {
					//line
					int wordlen = 2;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingStartWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingStartWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingStartWord[i] = true;
								}
							}
						}
					}

					//m_bFoldingPossible[i] = true;
				}
				if ((found = (char*)pestrcasestr(bufferBegin, "loop"))  ) {
					int wordlen = 4;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingEndWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingEndWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingEndWord[i] = true;
								}
							}
						}
					}
					//m_bFoldingPossible[i] = true;
				}
				//FUNCTION - ENDFUNCTION
				if ((found = (char*)pestrcasestr(bufferBegin, "endfunction"))) {
					int wordlen = 11;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingEndWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingEndWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingEndWord[i] = true;
								}
							}
						}
					}
				}
				if ((found = (char*)pestrcasestr(bufferBegin, "function"))) {
					//line
					int wordlen = 8;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingStartWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {

							//PE: rep this in all functions.
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingStartWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if ((line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' '))
										m_bFoldingStartWord[i] = true;
								}
							}
						}
					}
				}
				//REMSTART - REMEND
				if ((found = (char*)pestrcasestr(bufferBegin, "remend"))) {
					int wordlen = 6;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment ) {
							m_bFoldingEndWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex != PaletteIndex::Comment ) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingEndWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingEndWord[i] = true;
								}
							}
						}
					}
				}
				if ((found = (char*)pestrcasestr(bufferBegin, "remstart"))) {
					//line
					int wordlen = 8;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment ) {
							m_bFoldingStartWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex != PaletteIndex::Comment ) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingStartWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingStartWord[i] = true;
								}
							}
						}
					}
				}
				//SELECT - ENDSELECT
				if ((found = (char*)pestrcasestr(bufferBegin, "endselect"))) {
					int wordlen = 9;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingEndWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingEndWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingEndWord[i] = true;
								}
							}
						}
					}
				}
				if ((found = (char*)pestrcasestr(bufferBegin, "select"))) {
					//line
					int wordlen = 6;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingStartWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingStartWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingStartWord[i] = true;
								}
							}
						}
					}
				}
				//CASE - ENDCASE
				if ((found = (char*)pestrcasestr(bufferBegin, "endcase"))) {
					int wordlen = 7;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingEndWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingEndWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingEndWord[i] = true;
								}
							}
						}
					}
				}
				if ((found = (char*)pestrcasestr(bufferBegin, "case"))) {
					//line
					int wordlen = 4;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingStartWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingStartWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingStartWord[i] = true;
								}
							}
						}
					}
				}

				//FOR  - NEXT
				if ((found = (char*)pestrcasestr(bufferBegin, "next"))) {
					int wordlen = 4;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingEndWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingEndWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingEndWord[i] = true;
								}
							}
						}
					}
				}
				if ((found = (char*)pestrcasestr(bufferBegin, "for"))) {
					//line
					int wordlen = 3;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingStartWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingStartWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingStartWord[i] = true;
								}
							}
						}
					}
				}


				//TYPE - ENDTYPE
				if ((found = (char*)pestrcasestr(bufferBegin, "endtype"))) {
					int wordlen = 7;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingEndWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingEndWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingEndWord[i] = true;
								}
							}
						}
					}
				}
				if ((found = (char*)pestrcasestr(bufferBegin, "type"))) {
					//line
					int wordlen = 4;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingStartWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingStartWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingStartWord[i] = true;
								}
							}
						}
					}
				}


				//REPEAT  - UNTIL
				if ((found = (char*)pestrcasestr(bufferBegin, "until"))) {
					int wordlen = 5;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingEndWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingEndWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingEndWord[i] = true;
								}
							}
						}
					}
				}
				if ((found = (char*)pestrcasestr(bufferBegin, "repeat"))) {
					//line
					int wordlen = 6;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingStartWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingStartWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingStartWord[i] = true;
								}
							}
						}
					}
				}


				//IF - ENDIF
				bool bIfFound = false;
				if ((found = (char*)pestrcasestr(bufferBegin, "if"))) {
					//line
					int wordlen = 2;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
//						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
//							m_bFoldingStartWord[i] = true;
//							bIfFound = true;
//						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
//									m_bFoldingStartWord[i] = true;
//									bIfFound = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ' || line[pos + wordlen].mChar == '(') {
										m_bFoldingStartWord[i] = true;
										bIfFound = true;
									}
								}
							}
						}
					}
				}
				if (bIfFound) {
					//Then must cancel startword.
					if ((found = (char*)pestrcasestr(bufferBegin, "then"))) {
						//line
						int wordlen = 4;
						int pos = found - bufferBegin;
						if (line.size() == wordlen) {
							if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
								m_bFoldingStartWord[i] = false;
							}
						}
						else if (pos >= 0 && pos + wordlen <= line.size()) {
							if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
								if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
									if (found + wordlen == bufferEnd) {
										m_bFoldingStartWord[i] = false;
									}
									else if (pos + wordlen < line.size()) {
										if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ') {
											m_bFoldingStartWord[i] = false;
										}
									}
								}
							}
						}
					}
				}

				if ((found = (char*)pestrcasestr(bufferBegin, "endif"))) {
					int wordlen = 5;
					int pos = found - bufferBegin;
					if (line.size() == wordlen) {
						if (line[0].mColorIndex != PaletteIndex::Comment && !line[0].mMultiLineComment) {
							m_bFoldingEndWord[i] = true;
						}
					}
					else if (pos >= 0 && pos + wordlen <= line.size()) {
						if (line[pos].mColorIndex == PaletteIndex::Keyword && line[pos].mColorIndex != PaletteIndex::Comment && !line[pos].mMultiLineComment) {
							if (!(pos > 0 && !(line[pos - 1].mChar == '\t' || line[pos - 1].mChar == ' '))) {
								if (found + wordlen == bufferEnd) {
									m_bFoldingEndWord[i] = true;
								}
								else if (pos + wordlen < line.size()) {
									if (line[pos + wordlen].mChar == '\t' || line[pos + wordlen].mChar == ' ')
										m_bFoldingEndWord[i] = true;
								}
							}
						}
					}
				}


				//PE: if we have a start and a end on same line disable it.
				if (m_bFoldingStartWord[i] && m_bFoldingEndWord[i]) {
					m_bFoldingStartWord[i] = false;
					m_bFoldingEndWord[i] = false;
				}


			}
		}

	}
}

//PE: changed to reuse ColorizeRange.
void TextEditor::FastColorizeRange(int aFromLine, int aToLine)
{
	if (mLines.empty() || aFromLine >= aToLine)
		return;
	if (aToLine >= MAXPRECHECKLINES) //max supported.
		return;
	if (aFromLine > MAXPRECHECKLINES || aToLine - 1 > MAXPRECHECKLINES)
		return;
	if (m_bFirstCheck[aFromLine] && m_bFirstCheck[aToLine-1]) // already first time check ?
		return;

	std::string buffer;
	std::cmatch results;
	std::string id;

	int endLine = std::max(0, std::min((int)mLines.size(), aToLine));
	
	for (int i = aFromLine; i < endLine; ++i)
	{
		if (m_bFirstCheck[i])
			continue;
		m_bFirstCheck[i] = true;

		ColorizeRange(i, i+1);
	}
}

//SET start to start of current edit line , then just increase. for FAST change!
#define MAXCHECKLINESPERSYNC 1000
//int maxlinesperSync = 3000; //First scan go quickly. MOVED to per source file.

//static int maxchecklines[10];
//static bool withinString[10];
//static TextEditor::Coordinates commentStart[10]; // = TextEditor::Coordinates(0, 0);
static auto start = TextEditor::Coordinates(0, 0);
static auto end = TextEditor::Coordinates(0, 0);


void TextEditor::ColorizeInternal(int lineNo, int lineMax)
{
	//PE: Need more optimizing , 
	//PE: If first run on all text is done , do this from int lineNo, int lineMax

	if (mLines.empty())
		return;

	if (1==1 || mCheckMultilineComments)
	{

		if (maxchecklines >= mLines.size()) {
			maxchecklines = 0;
			withinString = false;
			withinTypeDefination = false;
			commentStart = TextEditor::Coordinates(mLines.size(), 0);
			maxlinesperSync = 80; // Turn down processes per sync.
			iFoldeStarters = 0;
			bInFolding = false;
			bFoldNow = false;
			breakonnext = false;
//			if (mLines.size() > 5000 )
//				maxlinesperSync = 200; // Turn down processes per sync.
//			if (mLines.size() > 10000)
//				maxlinesperSync = 300; // Turn down processes per sync.
//			if (mLines.size() > 50000)
//				maxlinesperSync = 400; // Turn down processes per sync.
		}

		start = Coordinates(maxchecklines, 0);
		maxchecklines += maxlinesperSync;
		if (maxchecklines >= mLines.size())
			maxchecklines = mLines.size();
		end = Coordinates((int)maxchecklines, 0);

		int tester = 0;
		//static bool checkingcomments = false;

		for (auto i = start; i < end; Advance(i))
		{
			if( i.mColumn ==0)
				withinString = false;

			auto& line = mLines[i.mLine];

			if ( isAGKsource && pref.bEnableSymboldList && i.mColumn == 0) {
				if (bUpdateSymbolsFirstTime && i.mLine >= (mLines.size()-1) ) {
					//We cant use the same list to display as we use to update.
					bool bUpdateNow = true;

					//PE: Delay update if all variales has moved (pressed enter) and list is empty. Do a second scan then.
					//PE: isssue : https://github.com/TheGameCreators/AGK-Studio/issues/353

					if (symbolsList[updateSymbolslist].size() <= 0 && symbolsList[activeSymbolslist].size() > 0 ) {
						if (iSkibUpdateSymbols > 0) {
							bUpdateNow = false;
							iSkibUpdateSymbols--;
						}
					}
					if (CodeProperties[updateCodePropertieslist].size() <= 0 && CodeProperties[activeCodePropertieslist].size() > 0) {
						if (iSkibUpdateSymbols > 0) {
							bUpdateNow = false;
							iSkibUpdateSymbols--;
						}
					}

					if (bUpdateNow) {
						iSkibUpdateSymbols = 4;
						if (activeSymbolslist == 0) {
							activeSymbolslist = 1;
							activeCodePropertieslist = 1;
							updateSymbolslist = 0;
							updateCodePropertieslist = 0;
						}
						else {
							activeSymbolslist = 0;
							activeCodePropertieslist = 0;
							updateSymbolslist = 1;
							updateCodePropertieslist = 1;
						}
						activeFunctions = updateFunctions;
						updateFunctions = 0;
						activeVariables = updateVariables;
						updateVariables = 0;
						activeConstants = updateConstants;
						updateConstants = 0;
						activeTypes = updateTypes;
						updateTypes = 0;
					}
					symbolsList[updateSymbolslist].clear();
					CodeProperties[updateCodePropertieslist].clear();
					
//					bUpdateSymbolsFirstTime = false;
				}
			}

			if (!line.empty())
			{
				if (isAGKsource &&  pref.bEnableSymboldList && i.mColumn == 0)
				{
					if (bUpdateSymbolsFirstTime && !line[i.mColumn].mMultiLineComment ) {


						//To secure variables, later make check after parsing.
						if (line.size() < MAX_PATH) {

							//Convert to normal string to find symbols.
							char *found;
							int lineSize = 0;
							int lineSize2 = 0;
							char * buffer = new char[line.size() + 2];
							char * buffer2 = new char[line.size() + 2];
							char * buffer3 = new char[line.size() + 2];
							
							if (buffer) buffer[line.size()+1] = 0;
							if (buffer2) buffer2[line.size() + 1] = 0;
							if (buffer3) buffer3[line.size() + 1] = 0;

							bool aftercomment = false;
							if (buffer) {
								for (size_t j = 0; j < line.size(); ++j)
								{
									auto& col = line[j];
									if (col.mColorIndex != PaletteIndex::Comment && !aftercomment) {
										buffer[lineSize] = col.mChar;
//										if (buffer[lineSize] == '\t' || buffer[lineSize] == ' ' || buffer[lineSize] == ':' || buffer[lineSize] == '\n')
										if (buffer[lineSize] == '\t' || buffer[lineSize] == ' ' || buffer[lineSize] == '\n')
											buffer[lineSize] = ';'; // ; = noise.

										if (buffer3) {
											buffer3[lineSize] = col.mChar;
											if (buffer3[lineSize] == '\t' || buffer3[lineSize] == ' ' || buffer3[lineSize] == '\n')
												buffer3[lineSize] = ';'; // ; = noise.
										}
										if (j < (line.size() - 1) && col.mColorIndex == PaletteIndex::Comment && line[j].mChar == '/' && line[j + 1].mChar == '/')
											aftercomment = true;
										else
										lineSize++;
										
										if (buffer2) {
											buffer2[lineSize2] = col.mChar;
											if (buffer2[lineSize2] == '\t' || buffer2[lineSize2] == ' ' || buffer2[lineSize2] == '\n')
												buffer2[lineSize2] = ';'; // ; = noise.
											lineSize2++;
										}

									}
									else {
										if (buffer2) {
											buffer2[lineSize2] = col.mChar;
											if (buffer2[lineSize2] == '\t' || buffer2[lineSize2] == ' ' || buffer2[lineSize2] == '\n')
												buffer2[lineSize2] = ';'; // ; = noise.
											lineSize2++;
										}
									}
								}
								if (buffer) buffer[lineSize] = 0;
								if (buffer3) buffer3[lineSize] = 0;
								if (buffer2) buffer2[lineSize2] = 0;
							}

							if (buffer && lineSize > 2) {
								
								//PE: 26-02-2019 pickup changes.
								found = (char*)pestrcasestr(&buffer[0], "remstart");
								if (found) {
									if(i.mLine < MAXPRECHECKLINES )
										m_bCheckLine[i.mLine] = true;
								}
								found = (char*)pestrcasestr(&buffer[0], "remend");
								if (found) {
									if (i.mLine < MAXPRECHECKLINES)
										m_bCheckLine[i.mLine] = true;
								}
								//if (strstr(bufferBegin, "//*") == NULL) {

								//Must ignore everything inside type defination.
								found = (char*)pestrcasestr(&buffer[0], "endtype");
								int pos;
								if (found) {
									pos = found - &buffer[0];
									if (pos > 0) {
										//Make sure endtype is alone.
										found = (char*)pestrcasestr(&buffer[0], ";endtype");
									}
								}
								if (found && withinTypeDefination) {
									iTypeLineNo = 999999;
									withinTypeDefination = false;
								}
								else {
									found = (char*)pestrcasestr(&buffer[0], "type;");
									int pos;
									if (found) {
										pos = found - &buffer[0];
										if (pos > 0) {
											//Make sure type is alone.
											found = (char*)pestrcasestr(&buffer[0], ";type;");
										}
									}
									if (found) {
										int pos = found - &buffer[0];
										if (pos >= 0 && pos < lineSize) {
											//+9
											uString szRest, szParm, szCmd;
											szRest.SetStr(&buffer[pos + 5]);
											//Clean noise.
											szRest.ReplaceStr(";", "");
											strcpy(cTypeName, szRest.GetStr());
											iTypeLineNo = i.mLine + 1;

											SymbolsValues stmp;
											stmp.iSymbolType = 4; //Types
											strcpy(stmp.cCommand, cTypeName);
											strcpy(stmp.cParameters, "");
											strcpy(stmp.cType, "");
											stmp.lineno = iTypeLineNo;
											stmp.bVisible = true;
											szRest.Lower();
											symbolsList[updateSymbolslist].insert(std::make_pair(std::string(szRest.GetStr()), stmp));
											updateTypes++;

										}
										withinTypeDefination = true;
									}
								}

								if (withinTypeDefination) {
									//Add blocks words for types with no endtype.
									if (i.mLine + 2 >= iTypeLineNo) {
										found = (char*)pestrcasestr(&buffer[0], ";if;");
										if (!found) found = (char*)pestrcasestr(&buffer[0], ";for;");
										if (!found) found = (char*)pestrcasestr(&buffer[0], "global;");
										if (!found) found = (char*)pestrcasestr(&buffer[0], "endif;");
										if (!found) found = (char*)pestrcasestr(&buffer[0], "function;");
										if (found || strstr(&buffer[0], "=")) {
											withinTypeDefination = false;
										}
									}
								}
							}

							//Find [IDEGUIADD]: entryes.
							//CodeProperties[updateCodePropertieslist]

							if (buffer2 && lineSize2 > 12) {
								//uString buf2 = buffer2;
								//buf2.Lower();
								//buf2.ReplaceStr("#export","[IDEGUIADD],")
								bool removeparmend = false;
								found = (char*)pestrcasestr(&buffer2[0], "[IDEGUIADD],");
								if (!found) {
									found = (char*)pestrcasestr(&buffer2[0], "#export");
									if (found) {
										char * tmp = (char*)pestrcasestr(&buffer2[0], "(");
										if (tmp) {
											tmp[0] = ';';
											removeparmend = true;
										}
										found[7] = ',';
										//if (buffer2[strlen(buffer2) - 1] == ')') buffer2[strlen(buffer2) - 1] = '\"';
									}
								}
								if (found) {
									uString szRest, szParm = "", szCmd = "";

									char *found2;
									found2 = (char*)pestrcasestr(&buffer3[0], "#export");
									if (found2) {
										found2[0] = 0;
									}

									szRest.SetStr(&buffer3[0]);
									szCmd = szRest;
									szParm.SetStr(&buffer2[0]);
									if (removeparmend) {
										int remto = szParm.RevFind(')');
										if (remto > 0) {
											uString tmp = szParm;
											tmp.SubString(szParm, 0, remto);
										}
									}

									szParm.ReplaceStr(",float;,", ",float,");
									szParm.ReplaceStr(";float,", ",float,");
									szParm.ReplaceStr(";float;,", ",float,");

									szParm.ReplaceStr(",integer;,", ",integer,");
									szParm.ReplaceStr(";integer,", ",integer,");
									szParm.ReplaceStr(";integer;,", ",integer,");

									szParm.ReplaceStr(",string;,", ",string,");
									szParm.ReplaceStr(";string,", ",string,");
									szParm.ReplaceStr(";string;,", ",string,");

									szParm.ReplaceStr(",selectfile;,", ",selectfile,");
									szParm.ReplaceStr(";selectfile,", ",selectfile,");
									szParm.ReplaceStr(";selectfile;,", ",selectfile,");

									szParm.ReplaceStr(",selectfolder;,", ",selectfolder,");
									szParm.ReplaceStr(";selectfolder,", ",selectfolder,");
									szParm.ReplaceStr(";selectfolder;,", ",selectfolder,");

									szParm.ReplaceStr(",vec4color;,", ",vec4color,");
									szParm.ReplaceStr(";vec4color,", ",vec4color,");
									szParm.ReplaceStr(";vec4color;,", ",vec4color,");


									szParm.ReplaceStr(",vec3;,", ",vec3,");
									szParm.ReplaceStr(";vec3,", ",vec3,");
									szParm.ReplaceStr(";vec3;,", ",vec3,");

									szParm.ReplaceStr(",vec4;,", ",vec4,");
									szParm.ReplaceStr(";vec4,", ",vec4,");
									szParm.ReplaceStr(";vec4;,", ",vec4,");

									szParm.ReplaceStr(",header;,", ",header,");
									szParm.ReplaceStr(";header,", ",header,");
									szParm.ReplaceStr(";header;,", ",header,");

									szParm.ReplaceStr(",message;,", ",message,");
									szParm.ReplaceStr(";message,", ",message,");
									szParm.ReplaceStr(";message;,", ",message,");

									szParm.ReplaceStr(",separator;,", ",separator,");
									szParm.ReplaceStr(";separator,", ",separator,");
									szParm.ReplaceStr(";separator;,", ",separator,");
									szParm.ReplaceStr(";separator;", ",separator,");
									szParm.ReplaceStr(";separator)", ",separator,");
									szParm.ReplaceStr(",separator)", ",separator,");
									szParm.ReplaceStr("(separator)", ",separator,");
									szParm.ReplaceStr(",separator;)", ",separator,");
									szParm.ReplaceStr(",separator", ",separator,");

									//Make sure its valid
									bool valid = false;
									if (!valid && pestrcasestr(szParm.GetStr(), "float")) valid = true;
									if (!valid && pestrcasestr(szParm.GetStr(), "integer")) valid = true;
									if (!valid && pestrcasestr(szParm.GetStr(), "string")) valid = true;
									if (!valid && pestrcasestr(szParm.GetStr(), "separator")) valid = true;
									if (!valid && pestrcasestr(szParm.GetStr(), "message")) valid = true;
									if (!valid && pestrcasestr(szParm.GetStr(), "header")) valid = true;
									if (!valid && pestrcasestr(szParm.GetStr(), "vec4")) valid = true;
									if (!valid && pestrcasestr(szParm.GetStr(), "vec3")) valid = true;
									if (!valid && pestrcasestr(szParm.GetStr(), "vec4color")) valid = true;
									if (!valid && pestrcasestr(szParm.GetStr(), "selectfolder")) valid = true;
									if (!valid && pestrcasestr(szParm.GetStr(), "selectfile")) valid = true;
									if (!valid && pestrcasestr(szParm.GetStr(), "variable")) valid = true;
									if (valid) {
										SymbolsValues stmp;
										stmp.iSymbolType = 1; //Code Properties.
										strcpy(stmp.cCommand, ";");
										strcat(stmp.cCommand, szCmd.GetStr());
										strcpy(stmp.cParameters, szParm.GetStr());
										strcpy(stmp.cType, "");
										stmp.bVisible = true;
										stmp.lineno = i.mLine + 1;
										if (szParm.GetLength() > 0) {
											//szCmd.Lower();
											CodeProperties[updateCodePropertieslist].insert(std::make_pair(std::int32_t(i.mLine), stmp));
										}
									}

								}
							}

							//Gosub blocks.
							if (buffer && lineSize > 2) {
								char *tmp = &buffer3[0];
								while (tmp[0] != 0 && tmp[0] == ';')
									tmp++;
								while (tmp[0] != 0 && strchr(ISALETTERDIGIT, tmp[0]))
									tmp++;
								if (tmp[0] != 0 && tmp[0] == ':' && tmp > &buffer3[0]) {
									//Gosub block.
									uString szLine, szFunction;
									int pos = tmp - &buffer3[0];
									szLine = &buffer3[0];
									if (pos > 0 && pos < lineSize) {
										szLine.SubString(szFunction, 0, pos);
										szFunction.ReplaceStr(";", "");
										//szFunction.Append("()");
										if (szFunction.GetLength() < MAX_PATH) {
											SymbolsValues stmp;
											stmp.iSymbolType = 1; //Function.
											strcpy(stmp.cCommand, szFunction.GetStr());
											strcpy(stmp.cParameters, ":");
											strcpy(stmp.cType, "");
											stmp.bVisible = true;
											stmp.lineno = i.mLine + 1;
											//Use command as key.
											if (szFunction.GetLength() > 0) {
												szFunction.Lower();
												szFunction.Append("##func");
												symbolsList[updateSymbolslist].insert(std::make_pair(std::string(szFunction.GetStr()), stmp));
												updateFunctions++;
											}
										}
									}
								}
							}

							//Loop lines.
							//char * buffer = new char[line.size() + 1];
							char *oldbuffer = buffer;


							bool foundcolon = true;
							char *cfoundcolon;

							while (foundcolon && buffer) {
								cfoundcolon = (char*)strstr(buffer, ":");
								if (cfoundcolon)
									cfoundcolon[0] = 0;

								if (buffer && lineSize > 2) {
									bool inFunction = false;

									//Find functions.
									found = (char*)pestrcasestr(&buffer[0], "function;");
									if (found > &buffer[0]) {
										//Bug: ignore endfunction and exitfunction.
										if (*(found - 1) == 't' || *(found - 1) == 'T' || *(found - 1) == 'd' || *(found - 1) == 'D' ) {
											found = NULL;
										}
									}
									if (!withinTypeDefination && found) {
										inFunction = true;
										int pos = found - &buffer[0];

										if (pos >= 0 && pos < lineSize) {
											//+9
											uString szRest, szParm, szCmd;
											szRest.SetStr(&buffer[pos + 9]);
											//Clean szRest for noise.
											//szRest.ReplaceStr(";", "");
											//find last )
											int pos2 = szRest.RevFindStr(")");
											int pos3 = szRest.FindStr("(");
											if (pos2 > 0 && pos3 >= 0) {
												szRest.SubString(szParm, pos3, pos2 - pos3 + 1);
												szRest.SubString(szCmd, 0, pos3);
												szCmd.ReplaceStr(";", "");
												szParm.ReplaceStr("(;", "(");
												szParm.ReplaceStr(";)", ")");
												szParm.ReplaceStr(",;", ",");
												szParm.ReplaceStr(";,", ",");
												szParm.ReplaceStr(";", " ");
												//Create.
												//szCmd.Append("()");
												if (szCmd.GetLength() < MAX_PATH && szParm.GetLength() < MAX_PATH) {
													SymbolsValues stmp;
													stmp.iSymbolType = 1; //Function.
													strcpy(stmp.cCommand, szCmd.GetStr());
													strcpy(stmp.cParameters, szParm.GetStr());
													strcpy(stmp.cType, "");
													stmp.bVisible = true;
													stmp.lineno = i.mLine + 1;
													//Use command as key.
													if (szCmd.GetLength() > 0) {
														szCmd.Lower();
														szCmd.Append("##func");
														symbolsList[updateSymbolslist].insert(std::make_pair(std::string(szCmd.GetStr()), stmp));
														updateFunctions++;
													}
												}

											}
										}
									}

									//Find variables.
									int iCmpLength = 7;
									bool bAsRequired = true;
									if (!inFunction) {
										found = (char*)pestrcasestr(&buffer[0], "global;");
										if (!found) {
											found = (char*)pestrcasestr(&buffer[0], "local;");
											iCmpLength = 6;
											bAsRequired = true;
										}
										if (!found) {
											found = (char*)pestrcasestr(&buffer[0], "dim;");
											iCmpLength = 4;
											bAsRequired = false;
										}
										if (!found) {
											found = (char*)pestrcasestr(&buffer[0], ";as;");
											if (found) {
												//Only ;as; used, assume a dim defination.
												found = &buffer[0];
												iCmpLength = 0;
												bAsRequired = true;
											}
										}
										if (withinTypeDefination && i.mLine >= iTypeLineNo) {
											found = &buffer[0];
											iCmpLength = 0;
											bAsRequired = false;
										}
									}
									if (!inFunction && found) {
										int pos = found - &buffer[0];

										if (pos >= 0 && pos < lineSize) {
											//+8
											uString szRest, szParm, szCmd, szAs, szTmp;
											szRest.SetStr(&buffer[pos + iCmpLength]);
											int dimpos = szRest.FindStr("dim;", 1);
											if (dimpos >= 0) {
												szRest.ReplaceStr(";dim;", ""); //remove dim if this is a global
												szRest.ReplaceStr("dim;", ""); //remove dim if this is a global
												szRest.ReplaceStr("Dim;", ""); //remove dim if this is a global
												szRest.ReplaceStr("DIM;", ""); //remove dim if this is a global
												bAsRequired = false;
											}
											int posas = szRest.FindStr(";as;", 1);
											szAs = "";
											if (!bAsRequired || posas > 0) {
												if (bAsRequired)
													szRest.SubString(szCmd, 0, posas);
												else if (posas > 0)
													szRest.SubString(szCmd, 0, posas);
												if (posas > 0) {
													szRest.SubString(szAs, posas);
													int pos4 = szAs.RevFindStr("[");
													if (pos4 > 0) {
														szTmp = szAs;
														int pos5 = szAs.FindStr(";as;", 1);
														if (pos5 < 0)
															pos5 = 0;
														else
															pos5 += 4;
														szTmp.SubString(szAs, pos5, pos4 - pos5);
													}
													int pos5 = szAs.FindStr(";as;", 1);
													if (pos5 >= 0) {
														szTmp = szAs;
														szTmp.SubString(szAs, pos5 + 4);
													}

												}
												szParm = "";
												int pos2 = szRest.RevFindStr("[");
												if (!bAsRequired && pos2 >= 0 && posas < 0)
													szRest.SubString(szCmd, 0, pos2);

												if (pos2 > 0 && pos2 < posas) {
													szRest.SubString(szCmd, 0, pos2);
												}
												int pos3 = szRest.FindStr("]");
												if (pos2 > 0 && pos3 >= 0) {
													szRest.SubString(szParm, pos2, pos3 - pos2 + 1);
												}

												if (withinTypeDefination && szCmd.GetLength() <= 0) {
													//Just use raw if in type and we cant parse.
													szCmd = szRest;
												}
												//Clean szCmd for noise.
												szCmd.ReplaceStr(";", "");
												//Clean szParm for noise.
												szParm.ReplaceStr(";", "");
												//Clean As variable type.
												szAs.Lower();
												szAs.Append(";");
												int poslike = szAs.FindStr("=");
												if (poslike >= 0) {
													uString sztmp = szAs;
													sztmp.SubString(szAs, 0, poslike);
												}
												szAs.ReplaceStr("float;", "");
												szAs.ReplaceStr("integer;", "");
												szAs.ReplaceStr("string;", "");
												szAs.ReplaceStr(";", "");

												if (szCmd.GetLength() < MAX_PATH && szParm.GetLength() < MAX_PATH) {
													if (withinTypeDefination && i.mLine >= iTypeLineNo) {
														if ((strlen(cTypeName) + szCmd.GetLength()) < MAX_PATH && szCmd.GetLength() > 0) {
															uString nCmd;
															SymbolsValues stmp;
															stmp.iSymbolType = 4; //Types
															strcpy(stmp.cCommand, cTypeName);
															strcat(stmp.cCommand, ".");
															strcat(stmp.cCommand, szCmd.GetStr());
															strcpy(stmp.cParameters, szParm.GetStr());
															strcpy(stmp.cType, szAs.GetStr());
															stmp.lineno = i.mLine + 1;
															stmp.bVisible = true;
															nCmd = stmp.cCommand;
															nCmd.Lower();
															symbolsList[updateSymbolslist].insert(std::make_pair(std::string(nCmd.GetStr()), stmp));
															updateTypes++;

															//Also add variable to list. so we can lookup types inside types.
															//SymbolsValues stmp;
															stmp.iSymbolType = 2; //Variable
															strcpy(stmp.cCommand, szCmd.GetStr());
															strcpy(stmp.cParameters, szParm.GetStr());
															strcpy(stmp.cType, szAs.GetStr());
															stmp.lineno = i.mLine + 1;
															stmp.bVisible = false;
															//Use command as key.
															if (szCmd.GetLength() > 0) {
																szCmd.Lower();
																symbolsList[updateSymbolslist].insert(std::make_pair(std::string(szCmd.GetStr()), stmp));
																updateVariables++;
															}

														}

													}
													else if (!withinTypeDefination) {
														SymbolsValues stmp;
														stmp.iSymbolType = 2; //Variable
														strcpy(stmp.cCommand, szCmd.GetStr());
														strcpy(stmp.cParameters, szParm.GetStr());
														strcpy(stmp.cType, szAs.GetStr());
														stmp.bVisible = true;
														stmp.lineno = i.mLine + 1;
														//Use command as key.
														if (szCmd.GetLength() > 0) {
															szCmd.Lower();
															symbolsList[updateSymbolslist].insert(std::make_pair(std::string(szCmd.GetStr()), stmp));
															updateVariables++;
														}
													}
												}
											}
										}
									} //global

									if (!withinTypeDefination && !inFunction) {
										//CONSTANTS
										found = (char*)pestrcasestr(&buffer[0], "#constant;");
										if (!inFunction && found) {
											int pos = found - &buffer[0];
											if (pos >= 0 && pos < lineSize) {
												//+8
												uString szRest, szParm, szCmd;
												szRest.SetStr(&buffer[pos + iCmpLength]);
												szRest.ReplaceStr(";;", ";"); //Remove all double noise.
												szRest.ReplaceStr(";;", ";"); //Again remove all double noise.
												szRest.ReplaceStr(";;", ";"); //Again remove all double noise.
												int pos2 = szRest.FindStr(";");
												if (pos2 > 0) {
													szRest.SubString(szCmd, pos2 + 1);
													szRest = szCmd;
													szCmd = "";
												}
												pos2 = szRest.FindStr(";");
												if (pos2 > 0) {
													szRest.SubString(szCmd, 0, pos2);
													szRest.SubString(szParm, pos2 + 1);
													//Clean szCmd for noise.
													szCmd.ReplaceStr(";", "");
													//Clean szParm for noise.
													szParm.ReplaceStr(";", "");

													if (szCmd.GetLength() < MAX_PATH && szParm.GetLength() < MAX_PATH) {
														SymbolsValues stmp;
														stmp.iSymbolType = 3; //Constant
														strcpy(stmp.cCommand, szCmd.GetStr());
														strcpy(stmp.cParameters, szParm.GetStr());
														strcpy(stmp.cType, "");
														stmp.bVisible = true;
														stmp.lineno = i.mLine + 1;
														//Use command as key.
														if (szCmd.GetLength() > 0) {
															szCmd.Lower();
															symbolsList[updateSymbolslist].insert(std::make_pair(std::string(szCmd.GetStr()), stmp));
															updateConstants++;
														}
													}

												}
											}
										}
									}

								}

								if (cfoundcolon) {
									buffer = cfoundcolon + 1;
									if (buffer >= oldbuffer + lineSize) {
										buffer = NULL;
										foundcolon = false;
									}
									else
										foundcolon = true;
								}
								else
									foundcolon = false;
							}
							buffer = oldbuffer;

							if( buffer ) 
								delete[] buffer;
							if (buffer2)
								delete[] buffer2;
							if (buffer3)
								delete[] buffer3;
						}
					}
				}

				//Check if we have a comment change active at this line. (m_bCheckLine used for speed reasons).
				if (checkingcomments == false && i.mLine < MAXPRECHECKLINES && m_bCheckLine[i.mLine] == false) {
					line[i.mColumn].mMultiLineComment = false;
					continue;
				}
				checkingcomments = true;
				tester++;


				auto& g = line[i.mColumn];
				auto c = g.mChar;

				bool inComment = commentStart <= i;

				if (withinString)
				{
					line[i.mColumn].mMultiLineComment = inComment;

					if (c == '\"')
					{
						if (i.mColumn + 1 < (int)line.size() && line[i.mColumn + 1].mChar == '\"')
						{
							Advance(i);
							if (i.mColumn < (int)line.size())
								line[i.mColumn].mMultiLineComment = inComment;
						}
						else
							withinString = false;
					}
					else if (c == '\\')
					{
						Advance(i);
						if (i.mColumn < (int)line.size()) {

							if (isAGKsource && line[i.mColumn].mChar == '\"' ) {
								// \" dont work as " only in AGK.
								withinString = false;
							}
							else {
								line[i.mColumn].mMultiLineComment = inComment;
							}
						}
					}
				}
				else
				{
					//PE: 26-02-2019: ignore string with in comments.
					if (c == '\"' && !line[i.mColumn].mMultiLineComment )
					{
						withinString = true;
						line[i.mColumn].mMultiLineComment = inComment;
					}
					else
					{

						auto pred = [](const char& a, const Glyph& b) { return tolower(a) == tolower(b.mChar); };
						auto from = line.begin() + i.mColumn;
						//from
						auto& startStr = mLanguageDefinition.mCommentStart;
						if (i.mColumn + startStr.size() <= line.size() && equals(startStr.begin(), startStr.end(), from, from + startStr.size(), pred)) {
							// add remstart
							if (i.mColumn > 0) {
								if (line[i.mColumn - 1].mColorIndex != PaletteIndex::Comment)
									commentStart = i;
							}
							else {
								commentStart = i;
							}
						}
						else {
							auto startStr = (std::string) "remstart";
							if (i.mColumn + startStr.size() <= line.size() && equals(startStr.begin(), startStr.end(), from, from + startStr.size(), pred)) {
								if (i.mColumn > 0) {
									if (line[i.mColumn - 1].mColorIndex != PaletteIndex::Comment)
										commentStart = i;
								}
								else {
									commentStart = i;
								}
							}
						}

						inComment = commentStart <= i;

						line[i.mColumn].mMultiLineComment = inComment;
						
						auto& endStr = mLanguageDefinition.mCommentEnd;
						if (i.mColumn + 1 >= (int)endStr.size() &&
							equals(endStr.begin(), endStr.end(), from + 1 - endStr.size(), from + 1, pred)) {
							commentStart = TextEditor::Coordinates(mLines.size(), 0); //end;
							checkingcomments = false;
						}
						else {
							auto endStr = (std::string) "remend";
//							if (i.mColumn + 1 >= (int)endStr.size() &&
//								equals(endStr.begin(), endStr.end(), from + 1 - endStr.size(), from + 1, pred)) {

							if (i.mColumn + 1 >= (int)endStr.size() &&
								equals(endStr.begin(), endStr.end(), from + 1 - endStr.size(), from + 1, pred)) {

								if (i.mColumn > 0) {
									if (line[i.mColumn - 1].mColorIndex != PaletteIndex::Comment) {
										commentStart = TextEditor::Coordinates(mLines.size(), 0); //end;
										checkingcomments = false;
									}
								}
								else {
									commentStart = TextEditor::Coordinates(mLines.size(), 0); //end;
									checkingcomments = false;
								}

							}

						}

					}
				}
			}
		}
		mCheckMultilineComments = false;
	}

	//Fast first time coloring of visual area.
	FastColorizeRange(lineNo, lineMax);

	if (mColorRangeMin < mColorRangeMax)
	{
#ifdef _DEBUG
		//PE: Regex is REALLY slow in debug mode, so lower per sync updates when debugging.
//		int increment = (mLanguageDefinition.mTokenize == nullptr) ? 50 : 10000;
		int increment = (mLanguageDefinition.mTokenize == nullptr) ? 50 : 10000;
		if (maxlinesperSync > 500) {
			increment = 4000;
		}
#else
		//Get something done in the first runs.
		int increment = (mLanguageDefinition.mTokenize == nullptr) ? 80 : 10000;
		if (maxlinesperSync > 500) {
			increment = 3000;
		}
#endif
		const int to = std::min(mColorRangeMin + increment, mColorRangeMax);
		ColorizeRange(mColorRangeMin, to);
		mColorRangeMin = to;

		if (mColorRangeMax == mColorRangeMin)
		{
			mColorRangeMin = std::numeric_limits<int>::max();
			mColorRangeMax = 0;
		}
		return;
	}
	else {
		//Convert this to directly recolor insteasd of using mColorRangeMin,mColorRangeMax
		if (iColorLine < 0 || iColorLine >= mLines.size() - 2)
			iColorLine = 0;
		else
			iColorLine+=2;
//		Colorize(iColorLine, 2); // background scan.
//		m_bFirstCheck[iColorLine] = false;
//		m_bFirstCheck[iColorLine+1] = false;
//		FastColorizeRange(iColorLine, iColorLine+2);
		ColorizeRange(iColorLine, iColorLine + 2);
	}
}

float TextEditor::TextDistanceToLineStart(const Coordinates& aFrom) const
{
	if (aFrom.mLine > mLines.size()) return 0.0f;

	auto& line = mLines[aFrom.mLine];
	auto fontScale = ImGui::GetFontSize() / ImGui::GetFont()->FontSize;
	static std::string buffer;
	float spaceSize = ImGui::CalcTextSize(" ").x + 1.0f * fontScale;
	float distance = 0.0f;

	//PE: As CalcTextSize has roundings , we need to follow color changes to get the exact same size.

	auto prevColor = line.empty() ? PaletteIndex::Default : (line[0].mMultiLineComment ? PaletteIndex::MultiLineComment : line[0].mColorIndex);
	ImVec2 bufferOffset = {0,0};
	auto columnNo = 0;
	buffer.clear();

	for (size_t it = 0u; it < line.size() && it < (unsigned)aFrom.mColumn; ++it)
	{
		auto glyph = line[it];
		auto color = glyph.mMultiLineComment ? PaletteIndex::MultiLineComment : glyph.mColorIndex;

		if ((color != prevColor || glyph.mChar == '\t') && !buffer.empty())
		{
			auto textSize = ImGui::CalcTextSize(buffer.c_str());
			bufferOffset.x += textSize.x + 1.0f * fontScale;
			buffer.clear();
		}
		prevColor = color;

		if (glyph.mChar == '\t') {
			float oldx = bufferOffset.x;
			//bufferOffset.x = (1.0f * fontScale + std::floor((1.0f + bufferOffset.x)) / (float(mTabSize) * spaceSize)) * (float(mTabSize) * spaceSize);
			bufferOffset.x = (1.0f + std::floor((1.0f + bufferOffset.x)) / (float(mTabSize) * spaceSize)) * (float(mTabSize) * spaceSize);
			bufferOffset.x = roundByTabSize(bufferOffset.x, mTabSize, spaceSize,oldx);
		}
		else {
			//PE: Render using UTF8, all our text is stored in unicode.
			char utf8[4];
			unsigned int c = (unsigned char)glyph.mChar;
			if (CharToUtf8(&utf8[0], 2, c) == 2)
			{
				buffer.push_back(utf8[0]);
				buffer.push_back(utf8[1]);
			}
			else {
				buffer.push_back(glyph.mChar);
			}
		}
		++columnNo;
	}
	if (!buffer.empty())
	{
		auto textSize = ImGui::CalcTextSize(buffer.c_str());
		bufferOffset.x += textSize.x + 1.0f * fontScale;
	}

	return(bufferOffset.x);

/*	
	//CalcTextSize rounding so width 7.757 = 7.0.
	float tabSize = 0.0;

	char testlinesize[1024];
	testlinesize[0] = 0;
	int sizebuflen = 0;
	for (size_t it = 0u; it < line.size() && it < (unsigned)aFrom.mColumn; ++it)
	{
		if (line[it].mChar != '\t') {
			if (line.size() < 1024) {
				testlinesize[sizebuflen] = line[it].mChar;
				testlinesize[++sizebuflen] = 0;
			}
		}
		if (line[it].mChar == '\t')
		{

			distance = (1.0f * fontScale + std::floor((1.0f + distance)) / (float(mTabSize) * spaceSize)) * (float(mTabSize) * spaceSize);
			for (int a = 0; a < mTabSize; a++) {
				if (sizebuflen < 1023) {
					testlinesize[sizebuflen] = ' ';
					testlinesize[++sizebuflen] = 0;
				}
			}
			tabSize += (1.0f * fontScale + std::floor((0.0f)) / (float(mTabSize) * spaceSize)) * (float(mTabSize) * spaceSize);
//			tabSize += ((float(mTabSize) * spaceSize)); // -ImGui::CalcTextSize("\t").x);

		}
		else
		{
			char tempCString[2];
 			tempCString[0] = line[it].mChar;
			tempCString[1] = '\0';
			distance += ImGui::CalcTextSize(tempCString).x + 1.0f * fontScale;
//			distance += ImGui::CalcTextSize(tempCString).x + 0.95f * fontScale;
		}
	}
//	return distance;
	//	agk::Print(disttest.x);
	//	agk::Print(distance);
	if (line.size() < 1024) {
		ImVec2 disttest = ImGui::CalcTextSize(testlinesize);
//		disttest.x += tabSize;
		return disttest.x;
	}
	return distance;
*/
}

void TextEditor::EnsureCursorVisible( void )
{
	//PE: code folding , Support needed here!
	
	//if (!mWithinRender)
	if (!mWithinRender)
	{
		mScrollToCursor = true;
		return;
	}

	float scrollX = ImGui::GetScrollX();
	float scrollY = ImGui::GetScrollY();

	auto height = ImGui::GetWindowHeight();
	auto width = ImGui::GetWindowWidth();

	auto top = 1 + (int)ceil(scrollY / mCharAdvance.y);
	auto bottom = (int)ceil((scrollY + height) / mCharAdvance.y);

//	auto left = (int)ceil(scrollX / mCharAdvance.x); // PE: 26-03-2019
//	auto right = (int)ceil((scrollX+width) / mCharAdvance.x); // PE: 26-03-2019
	auto left = (int)ceil(scrollX ); // PE: 26-03-2019
	auto right = (int)ceil((scrollX + width) ); // PE: 26-03-2019

	auto pos = GetActualCursorCoordinates();
	if (mScrollToCursor && mScrollToCursorAddLines > 0) {
		pos.mLine += mScrollToCursorAddLines;
		if (pos.mLine < 1) pos.mLine = 0;
		mScrollToCursorAddLines = 0; //Only work one time.
	}
	auto len = TextDistanceToLineStart(pos);

	if (pref.iEnableCodeFolding && pos.mLine < MAXPRECHECKLINES) {

		//PE: 08-09-2019 NEED TO FIX THIS!
		//PE: 09-09-2019 This seams to work , need more testing!
		int newlineNo = pos.mLine;
		if(m_iHiddenLineNo[pos.mLine] > 0)
			pos.mLine = m_iHiddenLineNo[pos.mLine];

		if (pos.mLine < top + 2) {
			//newlineNo = pos.mLine - 3;
			//if (newlineNo > 0)
			//	newlineNo = m_iHiddenLineNo[pos.mLine - 3];
			//else
				//newlineNo = 0;
			if (newlineNo > 3)
				newlineNo = m_iHiddenLineNo[newlineNo] - 3;
			else
				newlineNo = 0;

			ImGui::SetScrollY(std::max(0.0f, newlineNo * mCharAdvance.y)); // -1
		}
		if (pos.mLine > bottom - 5) {
//			newlineNo = pos.mLine + 5;
//			if (pos.mLine + 5 > mLines.size() - 1)
//				newlineNo = m_iHiddenLineNo[mLines.size() - 1]+5;
//			else if (newlineNo > 0)
//				newlineNo = m_iHiddenLineNo[pos.mLine + 5];

			if (newlineNo + 5 > mLines.size() - 1) {
				if (m_iHiddenLineNo[mLines.size() - 1] > 0)
					newlineNo = m_iHiddenLineNo[mLines.size() - 1] + 5;
				else
					newlineNo = mLines.size() - 1 + 5;
			}
			else {
				if (m_iHiddenLineNo[newlineNo] > 0)
					newlineNo = newlineNo + 5;
			}

			ImGui::SetScrollY(std::max(0.0f, (newlineNo) * mCharAdvance.y - height)); //+4
		}
		//if (len + mTextStart < left + 4)
		if (len < left - 4)
			ImGui::SetScrollX(std::max(0.0f, len - mTextStart - 4));
		if (len + mTextStart + 44 > right - 4)
			ImGui::SetScrollX(len + mTextStart + 88 - width);
		if (pos.mColumn == 0) //PE: Let us see the line numbers when at first pos , and key home display correct.
			ImGui::SetScrollX(0.0);
	}
	else {
		if (pos.mLine < top + 2) //
			ImGui::SetScrollY(std::max(0.0f, (pos.mLine - 3) * mCharAdvance.y)); // -1
		if (pos.mLine > bottom - 5)
			ImGui::SetScrollY(std::max(0.0f, (pos.mLine + 5) * mCharAdvance.y - height)); //+4
		//if (len + mTextStart < left + 4)
		if (len < left - 4)
			ImGui::SetScrollX(std::max(0.0f, len - mTextStart - 4));
		if (len + mTextStart + 44 > right - 4)
			ImGui::SetScrollX(len + mTextStart + 88 - width);
		if (pos.mColumn == 0) //PE: Let us see the line numbers when at first pos , and key home display correct.
			ImGui::SetScrollX(0.0);
	}
}

int TextEditor::GetPageSize() const
{
	auto height = ImGui::GetWindowHeight() - 20.0f;
	return (int)floor(height / mCharAdvance.y);
}

TextEditor::UndoRecord::UndoRecord(
	const std::string& aAdded,
	const TextEditor::Coordinates aAddedStart,
	const TextEditor::Coordinates aAddedEnd,
	const std::string& aRemoved,
	const TextEditor::Coordinates aRemovedStart,
	const TextEditor::Coordinates aRemovedEnd,
	TextEditor::EditorState& aBefore,
	TextEditor::EditorState& aAfter)
	: mAdded(aAdded)
	, mAddedStart(aAddedStart)
	, mAddedEnd(aAddedEnd)
	, mRemoved(aRemoved)
	, mRemovedStart(aRemovedStart)
	, mRemovedEnd(aRemovedEnd)
	, mBefore(aBefore)
	, mAfter(aAfter)
{
	assert(mAddedStart <= mAddedEnd);
	assert(mRemovedStart <= mRemovedEnd);
}

void TextEditor::UndoRecord::Undo(TextEditor * aEditor)
{
	if (!mAdded.empty())
	{
		aEditor->DeleteRange(mAddedStart, mAddedEnd);
		aEditor->Colorize(mAddedStart.mLine - 1, mAddedEnd.mLine - mAddedStart.mLine + 2);
	}

	if (!mRemoved.empty())
	{
		auto start = mRemovedStart;
		aEditor->InsertTextAt(start, mRemoved.c_str());
		aEditor->Colorize(mRemovedStart.mLine - 1, mRemovedEnd.mLine - mRemovedStart.mLine + 2);
	}

	aEditor->mState = mBefore;
	aEditor->EnsureCursorVisible();

}

void TextEditor::UndoRecord::Redo(TextEditor * aEditor)
{
	if (!mRemoved.empty())
	{
		aEditor->DeleteRange(mRemovedStart, mRemovedEnd);
		aEditor->Colorize(mRemovedStart.mLine - 1, mRemovedEnd.mLine - mRemovedStart.mLine + 1);
	}

	if (!mAdded.empty())
	{
		auto start = mAddedStart;
		aEditor->InsertTextAt(start, mAdded.c_str());
		aEditor->Colorize(mAddedStart.mLine - 1, mAddedEnd.mLine - mAddedStart.mLine + 1);
	}

	aEditor->mState = mAfter;
	aEditor->EnsureCursorVisible();
}

static bool TokenizeCStyleComment(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	if (*in_begin != '/')
		return false;
	
	if (in_begin + 1 < in_end && in_begin[1] == '/')
	{
		out_begin = in_begin;
		out_end = in_end;
		return true;
	}
	
	return false;
}

static bool TokenizeCStylePreprocessorDirective(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	if (*in_begin != '#')
		return false;
	
	const char * p = in_begin + 1;
	
	while (p < in_end && isblank( (unsigned char) *p))
		p++;
	
	bool hasText = false;
	
	while (p < in_end && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_'))
	{
		hasText = true;
		p++;
	}
	
	if (hasText)
	{
		out_begin = in_begin;
		out_end = p;
		return true;
	}
	
	return false;
}

static bool TokenizeCStyleString(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	const char * p = in_begin;
	
	if (*p == '"')
	{
		p++;
		
		while (p < in_end)
		{
			// handle end of string
			if (*p == '"')
			{
				out_begin = in_begin;
				out_end = p + 1;
				return true;
			}
			
			// handle escape character for "
			if (*p == '\\' && p + 1 < in_end && p[1] == '"')
				p++;
			
			p++;
		}
	}
	
	return false;
}

static bool TokenizeCStyleCharacterLiteral(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	const char * p = in_begin;

	if (*p == '\'')
	{
		p++;
		
		// handle escape characters
		if (p < in_end && *p == '\\')
			p++;
		
		if (p < in_end)
			p++;
		
		// handle end of character literal
		if (p < in_end && *p == '\'')
		{
			out_begin = in_begin;
			out_end = p + 1;
			return true;
		}
	}

	return false;
}

static bool TokenizeAGKStyleIdentifier(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	const char * p = in_begin;

	if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_' || *p == '#' || *p == '$')
	{
		p++;

		while ((p < in_end) && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_' || *p == '#' || *p == '$' ))
			p++;

		out_begin = in_begin;
		out_end = p;
		return true;
	}

	return false;
}

static bool TokenizeCStyleIdentifier(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	const char * p = in_begin;
	
	if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
	{
		p++;
		
		while ((p < in_end) && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_'))
			p++;
		
		out_begin = in_begin;
		out_end = p;
		return true;
	}
	
	return false;
}

static bool TokenizeCStyleNumber(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	const char * p = in_begin;
	
	const bool startsWithNumber = *p >= '0' && *p <= '9';
	
	if (*p != '+' && *p != '-' && !startsWithNumber)
		return false;
	
	p++;
	
	bool hasNumber = startsWithNumber;
	
	while (p < in_end && (*p >= '0' && *p <= '9'))
	{
		hasNumber = true;
		
		p++;
	}
	
	if (hasNumber == false)
		return false;
	
	bool isFloat = false;
	bool isHex = false;
	bool isBinary = false;
	
	if (p < in_end)
	{
		if (*p == '.')
		{
			isFloat = true;
			
			p++;
			
			while (p < in_end && (*p >= '0' && *p <= '9'))
				p++;
		}
		else if (*p == 'x' || *p == 'X')
		{
			// hex formatted integer of the type 0xef80
			
			isHex = true;
			
			p++;
			
			while (p < in_end && ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')))
				p++;
		}
		else if (*p == 'b' || *p == 'B')
		{
			// binary formatted integer of the type 0b01011101
			
			isBinary = true;
			
			p++;
			
			while (p < in_end && (*p >= '0' && *p <= '1'))
				p++;
		}
	}
	
	if (isHex == false && isBinary == false)
	{
		// floating point exponent
		if (p < in_end && (*p == 'e' || *p == 'E'))
		{
			isFloat = true;
			
			p++;
			
			if (p < in_end && (*p == '+' || *p == '-'))
				p++;
			
			bool hasDigits = false;
			
			while (p < in_end && (*p >= '0' && *p <= '9'))
			{
				hasDigits = true;
				
				p++;
			}
			
			if (hasDigits == false)
				return false;
		}
		
		// single precision floating point type
		if (p < in_end && *p == 'f')
			p++;
	}
	
	if (isFloat == false)
	{
		// integer size type
		while (p < in_end && (*p == 'u' || *p == 'U' || *p == 'l' || *p == 'L'))
			p++;
	}
	
	out_begin = in_begin;
	out_end = p;
	return true;
}

static bool TokenizeCStylePunctuation(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	(void)in_end;

	switch (*in_begin)
	{
		case '[':
		case ']':
		case '{':
		case '}':
		case '!':
		case '%':
		case '^':
		case '&':
		case '*':
		case '(':
		case ')':
		case '-':
		case '+':
		case '=':
		case '~':
		case '|':
		case '<':
		case '>':
		case '?':
		case ':':
		case '/':
		case ';':
		case ',':
		case '.':
			out_begin = in_begin;
			out_end = in_begin + 1;
			return true;
	}
	
	return false;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::CPlusPlus()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const cppKeywords[] = {
			"alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept", "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char", "char16_t", "char32_t", "class",
			"compl", "concept", "const", "constexpr", "const_cast", "continue", "decltype", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false", "float",
			"for", "friend", "goto", "if", "import", "inline", "int", "long", "module", "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected", "public",
			"register", "reinterpret_cast", "requires", "return", "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "synchronized", "template", "this", "thread_local",
			"throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"
		};
		for (auto& k : cppKeywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh", "ctime", "div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit", "isgraph",
			"ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow", "printf", "sprintf", "snprintf", "putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand", "strcat", "strcmp", "strerror", "time", "tolower", "toupper",
			"std", "string", "vector", "map", "unordered_map", "set", "unordered_set", "min", "max"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}
		
		langDef.mTokenize = [](const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end, PaletteIndex & paletteIndex) -> bool
		{
			paletteIndex = PaletteIndex::Max;
			
			while (in_begin < in_end && isblank((unsigned char)*in_begin))
				in_begin++;
			
			if (in_begin == in_end)
			{
				out_begin = in_end;
				out_end = in_end;
				paletteIndex = PaletteIndex::Default;
			}
			else if (TokenizeCStyleComment(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Comment;
			else if (TokenizeCStylePreprocessorDirective(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Preprocessor;
			else if (TokenizeCStyleString(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::String;
			else if (TokenizeCStyleCharacterLiteral(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::CharLiteral;
			else if (TokenizeCStyleIdentifier(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Identifier;
			else if (TokenizeCStyleNumber(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Number;
			else if (TokenizeCStylePunctuation(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Punctuation;
			
			return paletteIndex != PaletteIndex::Max;
		};
		
		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = true;
		langDef.mAutoIndentation = true;

		langDef.mName = "C++";

		inited = true;
	}
	return langDef;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::HLSL()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"AppendStructuredBuffer", "asm", "asm_fragment", "BlendState", "bool", "break", "Buffer", "ByteAddressBuffer", "case", "cbuffer", "centroid", "class", "column_major", "compile", "compile_fragment",
			"CompileShader", "const", "continue", "ComputeShader", "ConsumeStructuredBuffer", "default", "DepthStencilState", "DepthStencilView", "discard", "do", "double", "DomainShader", "dword", "else",
			"export", "extern", "false", "float", "for", "fxgroup", "GeometryShader", "groupshared", "half", "Hullshader", "if", "in", "inline", "inout", "InputPatch", "int", "interface", "line", "lineadj",
			"linear", "LineStream", "matrix", "min16float", "min10float", "min16int", "min12int", "min16uint", "namespace", "nointerpolation", "noperspective", "NULL", "out", "OutputPatch", "packoffset",
			"pass", "pixelfragment", "PixelShader", "point", "PointStream", "precise", "RasterizerState", "RenderTargetView", "return", "register", "row_major", "RWBuffer", "RWByteAddressBuffer", "RWStructuredBuffer",
			"RWTexture1D", "RWTexture1DArray", "RWTexture2D", "RWTexture2DArray", "RWTexture3D", "sample", "sampler", "SamplerState", "SamplerComparisonState", "shared", "snorm", "stateblock", "stateblock_state",
			"static", "string", "struct", "switch", "StructuredBuffer", "tbuffer", "technique", "technique10", "technique11", "texture", "Texture1D", "Texture1DArray", "Texture2D", "Texture2DArray", "Texture2DMS",
			"Texture2DMSArray", "Texture3D", "TextureCube", "TextureCubeArray", "true", "typedef", "triangle", "triangleadj", "TriangleStream", "uint", "uniform", "unorm", "unsigned", "vector", "vertexfragment",
			"VertexShader", "void", "volatile", "while",
			"bool1","bool2","bool3","bool4","double1","double2","double3","double4", "float1", "float2", "float3", "float4", "int1", "int2", "int3", "int4", "in", "out", "inout",
			"uint1", "uint2", "uint3", "uint4", "dword1", "dword2", "dword3", "dword4", "half1", "half2", "half3", "half4",
			"float1x1","float2x1","float3x1","float4x1","float1x2","float2x2","float3x2","float4x2",
			"float1x3","float2x3","float3x3","float4x3","float1x4","float2x4","float3x4","float4x4",
			"half1x1","half2x1","half3x1","half4x1","half1x2","half2x2","half3x2","half4x2",
			"half1x3","half2x3","half3x3","half4x3","half1x4","half2x4","half3x4","half4x4",
		};
		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"abort", "abs", "acos", "all", "AllMemoryBarrier", "AllMemoryBarrierWithGroupSync", "any", "asdouble", "asfloat", "asin", "asint", "asint", "asuint",
			"asuint", "atan", "atan2", "ceil", "CheckAccessFullyMapped", "clamp", "clip", "cos", "cosh", "countbits", "cross", "D3DCOLORtoUBYTE4", "ddx",
			"ddx_coarse", "ddx_fine", "ddy", "ddy_coarse", "ddy_fine", "degrees", "determinant", "DeviceMemoryBarrier", "DeviceMemoryBarrierWithGroupSync",
			"distance", "dot", "dst", "errorf", "EvaluateAttributeAtCentroid", "EvaluateAttributeAtSample", "EvaluateAttributeSnapped", "exp", "exp2",
			"f16tof32", "f32tof16", "faceforward", "firstbithigh", "firstbitlow", "floor", "fma", "fmod", "frac", "frexp", "fwidth", "GetRenderTargetSampleCount",
			"GetRenderTargetSamplePosition", "GroupMemoryBarrier", "GroupMemoryBarrierWithGroupSync", "InterlockedAdd", "InterlockedAnd", "InterlockedCompareExchange",
			"InterlockedCompareStore", "InterlockedExchange", "InterlockedMax", "InterlockedMin", "InterlockedOr", "InterlockedXor", "isfinite", "isinf", "isnan",
			"ldexp", "length", "lerp", "lit", "log", "log10", "log2", "mad", "max", "min", "modf", "msad4", "mul", "noise", "normalize", "pow", "printf",
			"Process2DQuadTessFactorsAvg", "Process2DQuadTessFactorsMax", "Process2DQuadTessFactorsMin", "ProcessIsolineTessFactors", "ProcessQuadTessFactorsAvg",
			"ProcessQuadTessFactorsMax", "ProcessQuadTessFactorsMin", "ProcessTriTessFactorsAvg", "ProcessTriTessFactorsMax", "ProcessTriTessFactorsMin",
			"radians", "rcp", "reflect", "refract", "reversebits", "round", "rsqrt", "saturate", "sign", "sin", "sincos", "sinh", "smoothstep", "sqrt", "step",
			"tan", "tanh", "tex1D", "tex1D", "tex1Dbias", "tex1Dgrad", "tex1Dlod", "tex1Dproj", "tex2D", "tex2D", "tex2Dbias", "tex2Dgrad", "tex2Dlod", "tex2Dproj",
			"tex3D", "tex3D", "tex3Dbias", "tex3Dgrad", "tex3Dlod", "tex3Dproj", "texCUBE", "texCUBE", "texCUBEbias", "texCUBEgrad", "texCUBElod", "texCUBEproj", "transpose", "trunc"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("//.*", PaletteIndex::Comment));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[ \\t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = true;
		langDef.mAutoIndentation = true;

		langDef.mName = "HLSL";

		inited = true;
	}
	return langDef;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::GLSL()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict", "return", "short",
			"signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary",
			"_Noreturn", "_Static_assert", "_Thread_local"

			,"uniform","in","out","invariant","varying",
			//vertex build in
			"gl_PerVertex","gl_Position","gl_PointSize","gl_ClipDistance",
			//fragment build in
			"gl_FragCoord","gl_FrontFacing","gl_PointCoord","gl_FragDepth",
			"highp","mediump","lowp",
			"vec4","vec3","vec2","vec",
			"mat4","mat3","mat2",
			"sampler2D","attribute",
			"gl_FragColor",
		};
		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh", "ctime", "div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit", "isgraph",
			"ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow", "putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand", "strcat", "strcmp", "strerror", "time", "tolower", "toupper"
			,"texture2DLod","texture2D","clamp","mix","normalize"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("//.*", PaletteIndex::Comment));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[ \\t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = true;
		langDef.mAutoIndentation = true;

		langDef.mName = "GLSL";

		inited = true;
	}
	return langDef;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::C()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict", "return", "short",
			"signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary",
			"_Noreturn", "_Static_assert", "_Thread_local"
		};
		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"abort", "abs", "acos", "asin", "atan", "atexit", "atof", "atoi", "atol", "ceil", "clock", "cosh", "ctime", "div", "exit", "fabs", "floor", "fmod", "getchar", "getenv", "isalnum", "isalpha", "isdigit", "isgraph",
			"ispunct", "isspace", "isupper", "kbhit", "log10", "log2", "log", "memcmp", "modf", "pow", "putchar", "putenv", "puts", "rand", "remove", "rename", "sinh", "sqrt", "srand", "strcat", "strcmp", "strerror", "time", "tolower", "toupper"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenize = [](const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end, PaletteIndex & paletteIndex) -> bool
		{
			paletteIndex = PaletteIndex::Max;
			
			while (in_begin < in_end && isblank((unsigned char)*in_begin))
				in_begin++;
			
			if (in_begin == in_end)
			{
				out_begin = in_end;
				out_end = in_end;
				paletteIndex = PaletteIndex::Default;
			}
			else if (TokenizeCStyleComment(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Comment;
			else if (TokenizeCStylePreprocessorDirective(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Preprocessor;
			else if (TokenizeCStyleString(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::String;
			else if (TokenizeCStyleCharacterLiteral(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::CharLiteral;
			else if (TokenizeCStyleIdentifier(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Identifier;
			else if (TokenizeCStyleNumber(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Number;
			else if (TokenizeCStylePunctuation(in_begin, in_end, out_begin, out_end))
				paletteIndex = PaletteIndex::Punctuation;
			
			return paletteIndex != PaletteIndex::Max;
		};
		
		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = true;
		langDef.mAutoIndentation = true;

		langDef.mName = "C";

		inited = true;
	}
	return langDef;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::SQL()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"ADD", "EXCEPT", "PERCENT", "ALL", "EXEC", "PLAN", "ALTER", "EXECUTE", "PRECISION", "AND", "EXISTS", "PRIMARY", "ANY", "EXIT", "PRINT", "AS", "FETCH", "PROC", "ASC", "FILE", "PROCEDURE",
			"AUTHORIZATION", "FILLFACTOR", "PUBLIC", "BACKUP", "FOR", "RAISERROR", "BEGIN", "FOREIGN", "READ", "BETWEEN", "FREETEXT", "READTEXT", "BREAK", "FREETEXTTABLE", "RECONFIGURE",
			"BROWSE", "FROM", "REFERENCES", "BULK", "FULL", "REPLICATION", "BY", "FUNCTION", "RESTORE", "CASCADE", "GOTO", "RESTRICT", "CASE", "GRANT", "RETURN", "CHECK", "GROUP", "REVOKE",
			"CHECKPOINT", "HAVING", "RIGHT", "CLOSE", "HOLDLOCK", "ROLLBACK", "CLUSTERED", "IDENTITY", "ROWCOUNT", "COALESCE", "IDENTITY_INSERT", "ROWGUIDCOL", "COLLATE", "IDENTITYCOL", "RULE",
			"COLUMN", "IF", "SAVE", "COMMIT", "IN", "SCHEMA", "COMPUTE", "INDEX", "SELECT", "CONSTRAINT", "INNER", "SESSION_USER", "CONTAINS", "INSERT", "SET", "CONTAINSTABLE", "INTERSECT", "SETUSER",
			"CONTINUE", "INTO", "SHUTDOWN", "CONVERT", "IS", "SOME", "CREATE", "JOIN", "STATISTICS", "CROSS", "KEY", "SYSTEM_USER", "CURRENT", "KILL", "TABLE", "CURRENT_DATE", "LEFT", "TEXTSIZE",
			"CURRENT_TIME", "LIKE", "THEN", "CURRENT_TIMESTAMP", "LINENO", "TO", "CURRENT_USER", "LOAD", "TOP", "CURSOR", "NATIONAL", "TRAN", "DATABASE", "NOCHECK", "TRANSACTION",
			"DBCC", "NONCLUSTERED", "TRIGGER", "DEALLOCATE", "NOT", "TRUNCATE", "DECLARE", "NULL", "TSEQUAL", "DEFAULT", "NULLIF", "UNION", "DELETE", "OF", "UNIQUE", "DENY", "OFF", "UPDATE",
			"DESC", "OFFSETS", "UPDATETEXT", "DISK", "ON", "USE", "DISTINCT", "OPEN", "USER", "DISTRIBUTED", "OPENDATASOURCE", "VALUES", "DOUBLE", "OPENQUERY", "VARYING","DROP", "OPENROWSET", "VIEW",
			"DUMMY", "OPENXML", "WAITFOR", "DUMP", "OPTION", "WHEN", "ELSE", "OR", "WHERE", "END", "ORDER", "WHILE", "ERRLVL", "OUTER", "WITH", "ESCAPE", "OVER", "WRITETEXT"
		};

		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"ABS",  "ACOS",  "ADD_MONTHS",  "ASCII",  "ASCIISTR",  "ASIN",  "ATAN",  "ATAN2",  "AVG",  "BFILENAME",  "BIN_TO_NUM",  "BITAND",  "CARDINALITY",  "CASE",  "CAST",  "CEIL",
			"CHARTOROWID",  "CHR",  "COALESCE",  "COMPOSE",  "CONCAT",  "CONVERT",  "CORR",  "COS",  "COSH",  "COUNT",  "COVAR_POP",  "COVAR_SAMP",  "CUME_DIST",  "CURRENT_DATE",
			"CURRENT_TIMESTAMP",  "DBTIMEZONE",  "DECODE",  "DECOMPOSE",  "DENSE_RANK",  "DUMP",  "EMPTY_BLOB",  "EMPTY_CLOB",  "EXP",  "EXTRACT",  "FIRST_VALUE",  "FLOOR",  "FROM_TZ",  "GREATEST",
			"GROUP_ID",  "HEXTORAW",  "INITCAP",  "INSTR",  "INSTR2",  "INSTR4",  "INSTRB",  "INSTRC",  "LAG",  "LAST_DAY",  "LAST_VALUE",  "LEAD",  "LEAST",  "LENGTH",  "LENGTH2",  "LENGTH4",
			"LENGTHB",  "LENGTHC",  "LISTAGG",  "LN",  "LNNVL",  "LOCALTIMESTAMP",  "LOG",  "LOWER",  "LPAD",  "LTRIM",  "MAX",  "MEDIAN",  "MIN",  "MOD",  "MONTHS_BETWEEN",  "NANVL",  "NCHR",
			"NEW_TIME",  "NEXT_DAY",  "NTH_VALUE",  "NULLIF",  "NUMTODSINTERVAL",  "NUMTOYMINTERVAL",  "NVL",  "NVL2",  "POWER",  "RANK",  "RAWTOHEX",  "REGEXP_COUNT",  "REGEXP_INSTR",
			"REGEXP_REPLACE",  "REGEXP_SUBSTR",  "REMAINDER",  "REPLACE",  "ROUND",  "ROWNUM",  "RPAD",  "RTRIM",  "SESSIONTIMEZONE",  "SIGN",  "SIN",  "SINH",
			"SOUNDEX",  "SQRT",  "STDDEV",  "SUBSTR",  "SUM",  "SYS_CONTEXT",  "SYSDATE",  "SYSTIMESTAMP",  "TAN",  "TANH",  "TO_CHAR",  "TO_CLOB",  "TO_DATE",  "TO_DSINTERVAL",  "TO_LOB",
			"TO_MULTI_BYTE",  "TO_NCLOB",  "TO_NUMBER",  "TO_SINGLE_BYTE",  "TO_TIMESTAMP",  "TO_TIMESTAMP_TZ",  "TO_YMINTERVAL",  "TRANSLATE",  "TRIM",  "TRUNC", "TZ_OFFSET",  "UID",  "UPPER",
			"USER",  "USERENV",  "VAR_POP",  "VAR_SAMP",  "VARIANCE",  "VSIZE "
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\-\\-.*", PaletteIndex::Comment));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\\'[^\\\']*\\\'", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = false;
		langDef.mAutoIndentation = false;

		langDef.mName = "SQL";

		inited = true;
	}
	return langDef;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::AngelScript()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"and", "abstract", "auto", "bool", "break", "case", "cast", "class", "const", "continue", "default", "do", "double", "else", "enum", "false", "final", "float", "for",
			"from", "funcdef", "function", "get", "if", "import", "in", "inout", "int", "interface", "int8", "int16", "int32", "int64", "is", "mixin", "namespace", "not",
			"null", "or", "out", "override", "private", "protected", "return", "set", "shared", "super", "switch", "this ", "true", "typedef", "uint", "uint8", "uint16", "uint32",
			"uint64", "void", "while", "xor"
		};

		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"cos", "sin", "tab", "acos", "asin", "atan", "atan2", "cosh", "sinh", "tanh", "log", "log10", "pow", "sqrt", "abs", "ceil", "floor", "fraction", "closeTo", "fpFromIEEE", "fpToIEEE",
			"complex", "opEquals", "opAddAssign", "opSubAssign", "opMulAssign", "opDivAssign", "opAdd", "opSub", "opMul", "opDiv"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("//.*", PaletteIndex::Comment));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = true;
		langDef.mAutoIndentation = true;

		langDef.mName = "AngelScript";

		inited = true;
	}
	return langDef;
}

const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::Lua()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"and", "break", "do", "", "else", "elseif", "end", "false", "for", "function", "if", "in", "", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while"
		};

		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"assert", "collectgarbage", "dofile", "error", "getmetatable", "ipairs", "loadfile", "load", "loadstring",  "next",  "pairs",  "pcall",  "print",  "rawequal",  "rawlen",  "rawget",  "rawset",
			"select",  "setmetatable",  "tonumber",  "tostring",  "type",  "xpcall",  "_G",  "_VERSION","arshift", "band", "bnot", "bor", "bxor", "btest", "extract", "lrotate", "lshift", "replace", 
			"rrotate", "rshift", "create", "resume", "running", "status", "wrap", "yield", "isyieldable", "debug","getuservalue", "gethook", "getinfo", "getlocal", "getregistry", "getmetatable", 
			"getupvalue", "upvaluejoin", "upvalueid", "setuservalue", "sethook", "setlocal", "setmetatable", "setupvalue", "traceback", "close", "flush", "input", "lines", "open", "output", "popen", 
			"read", "tmpfile", "type", "write", "close", "flush", "lines", "read", "seek", "setvbuf", "write", "__gc", "__tostring", "abs", "acos", "asin", "atan", "ceil", "cos", "deg", "exp", "tointeger",
			"floor", "fmod", "ult", "log", "max", "min", "modf", "rad", "random", "randomseed", "sin", "sqrt", "string", "tan", "type", "atan2", "cosh", "sinh", "tanh",
			 "pow", "frexp", "ldexp", "log10", "pi", "huge", "maxinteger", "mininteger", "loadlib", "searchpath", "seeall", "preload", "cpath", "path", "searchers", "loaded", "module", "require", "clock",
			 "date", "difftime", "execute", "exit", "getenv", "remove", "rename", "setlocale", "time", "tmpname", "byte", "char", "dump", "find", "format", "gmatch", "gsub", "len", "lower", "match", "rep",
			 "reverse", "sub", "upper", "pack", "packsize", "unpack", "concat", "maxn", "insert", "pack", "unpack", "remove", "move", "sort", "offset", "codepoint", "char", "len", "codes", "charpattern",
			 "coroutine", "table", "io", "os", "string", "utf8", "bit32", "math", "debug", "package"
		};
		for (auto& k : identifiers)
		{
			Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\-\\-.*", PaletteIndex::Comment));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\\'[^\\\']*\\\'", PaletteIndex::String));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "\\-\\-\\[\\[";
		langDef.mCommentEnd = "\\]\\]";

		langDef.mCaseSensitive = true;
		langDef.mAutoIndentation = false;

		langDef.mName = "Lua";

		inited = true;
	}
	return langDef;
}



const TextEditor::LanguageDefinition& TextEditor::LanguageDefinition::AGK()
{
	static bool inited = false;
	static LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"and", "as", "case", "case", "", "", "default", "continue", "dec", "dim", "do", "else", "elseif", "end", "endcase", "endfunction", "endif", "endselect", "endtype", "endwhile", "exit", "exitfunction", "float", "foldstart", "foldend", "for", "function", "global", "gosub", "goto", "if", "inc", "integer", "local", "loop", "next", "not", "or", "redim", "ref", "repeat", "return", "select", "step", "string", "then", "to", "type", "undim", "until", "while",
//			"#constant","#include","#insert"
			"#include", "#insert", "#constant", "#option_explicit", "#company_name" , "#export"
		};

		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		
		for (auto& k : agk_identifiers)
		{
			Identifier id;
			id.mDeclaration = "AGK function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		//preprocessor=#include", "#insert", "#constant", "#option_explicit", "#company_name
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("//.*", PaletteIndex::Comment)); //PE: speed fixed
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", PaletteIndex::String)); //PE: speed fixed
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("\\'\\\\?[^\\']\\'", PaletteIndex::CharLiteral)); //PE: speed fixed
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", PaletteIndex::Number)); //PE: speed fixed.
		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", PaletteIndex::Identifier));

//To slow disabled.
//		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("REM .*", PaletteIndex::Comment));
//		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[ \\t]*#[ \\t]*[a-zA-Z_]+", PaletteIndex::Preprocessor));
//		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
//		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", PaletteIndex::Number));
//		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", PaletteIndex::Number));
//		langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", PaletteIndex::Punctuation));

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";

		langDef.mCaseSensitive = false;
		langDef.mAutoIndentation = true;

		langDef.mName = "AGK";

		inited = true;
	}
	return langDef;
}


