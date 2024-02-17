#ifndef _H_AGK_LIBRARY_COMMANDS
#define _H_AGK_LIBRARY_COMMANDS

#ifdef _MSC_VER
	#define DLL_EXPORT __declspec(dllexport)
#else
	#pragma GCC visibility push(hidden)
	#define DLL_EXPORT __attribute__((visibility("default")))
#endif

class cImage;
class cSprite;

extern char*(*AGKCommand0)( unsigned int );
extern void(*AGKCommand1)( char* );
extern float(*AGKCommand2)( float );
extern float(*AGKCommand3)( float );
extern float(*AGKCommand4)( float );
extern float(*AGKCommand5)( float );
extern void(*AGKCommand6)( int );
extern void(*AGKCommand7)( float, int );
extern void(*AGKCommand8)( int, int, int, int );
extern void(*AGKCommand9)( int );
extern void(*AGKCommand10)( float );
extern void(*AGKCommand11)( int, int );
extern void(*AGKCommand12)(  );
extern char*(*AGKCommand13)(  );
extern void(*AGKCommand14)( float, float, float, float );
extern void(*AGKCommand15)( int, int );
extern void(*AGKCommand16)( int );
extern int(*AGKCommand17)(  );
extern int(*AGKCommand18)(  );
extern void(*AGKCommand19)( int, int );
extern void(*AGKCommand20)( unsigned int, unsigned int );
extern int(*AGKCommand21)(  );
extern int(*AGKCommand22)(  );
extern float(*AGKCommand23)(  );
extern float(*AGKCommand24)(  );
extern float(*AGKCommand25)(  );
extern float(*AGKCommand26)(  );
extern float(*AGKCommand27)(  );
extern int(*AGKCommand28)(  );
extern int(*AGKCommand29)(  );
extern int(*AGKCommand30)(  );
extern void(*AGKCommand31)( unsigned int );
extern void(*AGKCommand32)( unsigned int );
extern void(*AGKCommand33)( unsigned int );
extern void(*AGKCommand34)( unsigned int );
extern void(*AGKCommand35)( float, float );
extern void(*AGKCommand36)( float );
extern void(*AGKCommand37)( int );
extern float(*AGKCommand38)(  );
extern float(*AGKCommand39)(  );
extern float(*AGKCommand40)(  );
extern void(*AGKCommand41)(  );
extern void(*AGKCommand42)(  );
extern void(*AGKCommand43)( float );
extern void(*AGKCommand44)( float );
extern void(*AGKCommand45)( float );
extern void(*AGKCommand46)( float );
extern void(*AGKCommand47)(  );
extern void(*AGKCommand48)(  );
extern void(*AGKCommand49)(  );
extern void(*AGKCommand50)(  );
extern void(*AGKCommand51)(  );
extern void(*AGKCommand52)(  );
extern void(*AGKCommand53)(  );
extern void(*AGKCommand54)( int );
extern float(*AGKCommand55)(  );
extern void(*AGKCommand56)(  );
extern float(*AGKCommand57)(  );
extern int(*AGKCommand58)(  );
extern float(*AGKCommand59)(  );
extern unsigned int(*AGKCommand60)(  );
extern char*(*AGKCommand61)(  );
extern int(*AGKCommand62)(  );
extern char*(*AGKCommand63)( const char* );
extern char*(*AGKCommand64)( const char* );
extern char*(*AGKCommand65)( const char* );
extern char*(*AGKCommand66)( const char* );
extern char*(*AGKCommand67)( const char* );
extern void(*AGKCommand68)( unsigned int );
extern unsigned int(*AGKCommand69)(  );
extern unsigned int(*AGKCommand70)( unsigned int, unsigned int );
extern int(*AGKCommand71)( int );
extern void(*AGKCommand72)( unsigned int );
extern int(*AGKCommand73)(  );
extern int(*AGKCommand74)( int, int );
extern float(*AGKCommand75)( float, float );
extern float(*AGKCommand76)( float, float );
extern void(*AGKCommand77)( int );
extern void(*AGKCommand78)( int );
extern void(*AGKCommand79)( int );
extern void(*AGKCommand80)( int );
extern float(*AGKCommand81)(  );
extern unsigned int(*AGKCommand82)(  );
extern unsigned int(*AGKCommand83)(  );
extern void(*AGKCommand84)( unsigned int );
extern void(*AGKCommand85)( unsigned int );
extern void(*AGKCommand86)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand87)( unsigned int, unsigned int, unsigned int );
extern cImage*(*AGKCommand88)( unsigned int );
extern unsigned int(*AGKCommand89)( unsigned int );
extern void(*AGKCommand90)( unsigned int, const char *, int );
extern void(*AGKCommand91)( unsigned int, const char* );
extern unsigned int(*AGKCommand92)( const char*, int );
extern unsigned int(*AGKCommand93)( const char* );
extern void(*AGKCommand94)( unsigned int, unsigned int, const char * );
extern unsigned int(*AGKCommand95)( unsigned int, const char * );
extern void(*AGKCommand96)( unsigned int, const char*, float, float, int );
extern unsigned int(*AGKCommand97)( const char*, float, float, int );
extern void(*AGKCommand98)( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int );
extern unsigned int(*AGKCommand99)( unsigned int, unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand100)( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int );
extern unsigned int(*AGKCommand101)( unsigned int, unsigned int, unsigned int, unsigned int );
extern unsigned int(*AGKCommand102)( unsigned int );
extern void(*AGKCommand103)( unsigned int );
extern void(*AGKCommand104)(  );
extern float(*AGKCommand105)( unsigned int );
extern float(*AGKCommand106)( unsigned int );
extern void(*AGKCommand107)( unsigned int, unsigned int );
extern void(*AGKCommand108)( unsigned int, unsigned int );
extern void(*AGKCommand109)( unsigned int, unsigned int );
extern void(*AGKCommand110)( unsigned int, unsigned int );
extern void(*AGKCommand111)( unsigned int, unsigned int, int, int, int, int );
extern void(*AGKCommand112)( unsigned int, int, int, int );
extern void(*AGKCommand113)( unsigned int, const char* );
extern unsigned int(*AGKCommand114)( float, float, float, float );
extern void(*AGKCommand115)( unsigned int, float, float, float, float );
extern char*(*AGKCommand116)( unsigned int );
extern unsigned int(*AGKCommand117)( unsigned int, int, int, int, int );
extern void(*AGKCommand118)( unsigned int, unsigned int, int, int, int, int );
extern void(*AGKCommand119)( unsigned int, int, int );
extern unsigned int(*AGKCommand120)(  );
extern unsigned int(*AGKCommand121)(  );
extern unsigned int(*AGKCommand122)(  );
extern unsigned int(*AGKCommand123)(  );
extern unsigned int(*AGKCommand124)(  );
extern unsigned int(*AGKCommand125)(  );
extern char*(*AGKCommand126)( unsigned int );
extern unsigned int(*AGKCommand127)( const char*, int );
extern void(*AGKCommand128)( unsigned int, float );
extern void(*AGKCommand129)( int );
extern cSprite*(*AGKCommand130)( unsigned int );
extern unsigned int(*AGKCommand131)( unsigned int );
extern void(*AGKCommand132)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand133)( unsigned int );
extern unsigned int(*AGKCommand134)( const char* );
extern void(*AGKCommand135)( unsigned int, const char* );
extern void(*AGKCommand136)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand137)( unsigned int );
extern void(*AGKCommand138)( unsigned int );
extern unsigned int(*AGKCommand139)(  );
extern unsigned int(*AGKCommand140)( unsigned int );
extern void(*AGKCommand141)( unsigned int );
extern void(*AGKCommand142)(  );
extern void(*AGKCommand143)( unsigned int, unsigned int );
extern void(*AGKCommand144)( unsigned int, unsigned int, int );
extern void(*AGKCommand145)( unsigned int, unsigned int, int );
extern void(*AGKCommand146)( unsigned int, float, float, float, float, float, float, float, float );
extern void(*AGKCommand147)( unsigned int );
extern void(*AGKCommand148)( unsigned int, float, float );
extern void(*AGKCommand149)( unsigned int, float, float );
extern void(*AGKCommand150)( unsigned int, float );
extern void(*AGKCommand151)( unsigned int, float );
extern void(*AGKCommand152)( unsigned int, float );
extern void(*AGKCommand153)( unsigned int, float );
extern void(*AGKCommand154)( unsigned int, int );
extern void(*AGKCommand155)( unsigned int, int, int );
extern void(*AGKCommand156)( unsigned int, int );
extern void(*AGKCommand157)( unsigned int, int );
extern void(*AGKCommand158)( unsigned int, int );
extern float(*AGKCommand159)( unsigned int );
extern float(*AGKCommand160)( unsigned int );
extern float(*AGKCommand161)( unsigned int );
extern float(*AGKCommand162)( unsigned int );
extern float(*AGKCommand163)( unsigned int );
extern float(*AGKCommand164)( unsigned int );
extern float(*AGKCommand165)( unsigned int );
extern float(*AGKCommand166)( unsigned int );
extern float(*AGKCommand167)( unsigned int );
extern float(*AGKCommand168)( unsigned int );
extern float(*AGKCommand169)( unsigned int );
extern float(*AGKCommand170)( unsigned int );
extern int(*AGKCommand171)( unsigned int, float, float );
extern int(*AGKCommand172)( float, float );
extern int(*AGKCommand173)( int, float, float );
extern int(*AGKCommand174)( unsigned int, float, float );
extern int(*AGKCommand175)( unsigned int );
extern unsigned int(*AGKCommand176)( unsigned int );
extern unsigned int(*AGKCommand177)( unsigned int );
extern unsigned int(*AGKCommand178)( unsigned int );
extern unsigned int(*AGKCommand179)( unsigned int );
extern int(*AGKCommand180)( unsigned int );
extern int(*AGKCommand181)( unsigned int );
extern int(*AGKCommand182)( unsigned int );
extern int(*AGKCommand183)( unsigned int );
extern float(*AGKCommand184)( unsigned int, int );
extern float(*AGKCommand185)( unsigned int, int );
extern int(*AGKCommand186)( unsigned int, float );
extern int(*AGKCommand187)( unsigned int, float );
extern float(*AGKCommand188)( unsigned int, float, float );
extern float(*AGKCommand189)( unsigned int, float, float );
extern float(*AGKCommand190)( unsigned int, float, float );
extern float(*AGKCommand191)( unsigned int, float, float );
extern void(*AGKCommand192)( unsigned int, float, float );
extern void(*AGKCommand193)( unsigned int, int, int, int );
extern void(*AGKCommand194)( unsigned int, unsigned int );
extern void(*AGKCommand195)( unsigned int );
extern void(*AGKCommand196)( unsigned int, float );
extern void(*AGKCommand197)( unsigned int );
extern void(*AGKCommand198)( unsigned int, float );
extern void(*AGKCommand199)( unsigned int, float, int );
extern void(*AGKCommand200)( unsigned int, float, int, int, int );
extern void(*AGKCommand201)( unsigned int );
extern void(*AGKCommand202)( unsigned int );
extern void(*AGKCommand203)( unsigned int, int );
extern int(*AGKCommand204)( unsigned int );
extern int(*AGKCommand205)( unsigned int );
extern void(*AGKCommand206)( unsigned int, int );
extern void(*AGKCommand207)( unsigned int, float, float );
extern void(*AGKCommand208)( unsigned int, int, int, int, int );
extern void(*AGKCommand209)( unsigned int, int );
extern void(*AGKCommand210)( unsigned int, int );
extern void(*AGKCommand211)( unsigned int, int );
extern void(*AGKCommand212)( unsigned int, int );
extern void(*AGKCommand213)( unsigned int, float );
extern void(*AGKCommand214)( unsigned int, float, float );
extern void(*AGKCommand215)( unsigned int, float, float );
extern void(*AGKCommand216)( unsigned int, int );
extern void(*AGKCommand217)( unsigned int, float, float );
extern void(*AGKCommand218)( unsigned int, float, float );
extern void(*AGKCommand219)( unsigned int, float, float, float, float );
extern void(*AGKCommand220)( unsigned int, unsigned int );
extern void(*AGKCommand221)( unsigned int );
extern void(*AGKCommand222)( float );
extern void(*AGKCommand223)( float, float );
extern void(*AGKCommand224)(  );
extern void(*AGKCommand225)(  );
extern void(*AGKCommand226)( int );
extern void(*AGKCommand227)( int );
extern void(*AGKCommand228)( int );
extern float(*AGKCommand229)(  );
extern int(*AGKCommand230)(  );
extern void(*AGKCommand231)( int );
extern void(*AGKCommand232)( int );
extern void(*AGKCommand233)( int );
extern void(*AGKCommand234)( int );
extern void(*AGKCommand235)( int );
extern unsigned int(*AGKCommand236)( float, float, float, float, float, int );
extern void(*AGKCommand237)( unsigned int );
extern void(*AGKCommand238)( unsigned int, float, float );
extern void(*AGKCommand239)( unsigned int, float );
extern void(*AGKCommand240)( unsigned int, float );
extern void(*AGKCommand241)( unsigned int, int );
extern void(*AGKCommand242)( unsigned int, float, float, float, float, float );
extern void(*AGKCommand243)( unsigned int, float, float, float );
extern void(*AGKCommand244)( unsigned int, unsigned int, unsigned int, float, float );
extern void(*AGKCommand245)( unsigned int, float, float, float, float, float );
extern void(*AGKCommand246)( unsigned int, float, float, float );
extern void(*AGKCommand247)( unsigned int, unsigned int, unsigned int, float, float );
extern void(*AGKCommand248)( unsigned int );
extern void(*AGKCommand249)( unsigned int, float, float );
extern void(*AGKCommand250)( unsigned int );
extern void(*AGKCommand251)( unsigned int, int );
extern void(*AGKCommand252)( unsigned int );
extern void(*AGKCommand253)( unsigned int );
extern void(*AGKCommand254)( unsigned int, float );
extern void(*AGKCommand255)( unsigned int, float );
extern void(*AGKCommand256)( unsigned int, int );
extern void(*AGKCommand257)( unsigned int, float, float );
extern void(*AGKCommand258)( unsigned int, float );
extern void(*AGKCommand259)( unsigned int, float );
extern void(*AGKCommand260)( unsigned int, float );
extern void(*AGKCommand261)( unsigned int, int );
extern void(*AGKCommand262)( unsigned int, float );
extern void(*AGKCommand263)( unsigned int, int );
extern void(*AGKCommand264)( unsigned int, int );
extern void(*AGKCommand265)( unsigned int, unsigned int );
extern void(*AGKCommand266)( unsigned int, unsigned int, int );
extern void(*AGKCommand267)( unsigned int, unsigned int );
extern void(*AGKCommand268)( unsigned int, unsigned int, int );
extern void(*AGKCommand269)( unsigned int, float, float, float, float );
extern void(*AGKCommand270)( unsigned int, float );
extern void(*AGKCommand271)( unsigned int, float, float, float, float );
extern void(*AGKCommand272)( unsigned int, float );
extern float(*AGKCommand273)( unsigned int );
extern float(*AGKCommand274)( unsigned int );
extern float(*AGKCommand275)( unsigned int );
extern float(*AGKCommand276)( unsigned int );
extern void(*AGKCommand277)( unsigned int );
extern int(*AGKCommand278)( unsigned int );
extern void(*AGKCommand279)( unsigned int, unsigned int, unsigned int, float, float, float, float, int );
extern void(*AGKCommand280)( unsigned int, unsigned int, unsigned int, float, float, int );
extern void(*AGKCommand281)( unsigned int, unsigned int, unsigned int, float, float, float, float, int );
extern void(*AGKCommand282)( unsigned int, unsigned int, unsigned int, float, float, float, float, float, float, float, float, float, int );
extern void(*AGKCommand283)( unsigned int, unsigned int, float, float, float );
extern void(*AGKCommand284)( unsigned int, unsigned int, unsigned int, float, float, float, float, int );
extern void(*AGKCommand285)( unsigned int, unsigned int, unsigned int, float, float, int );
extern void(*AGKCommand286)( unsigned int, unsigned int, unsigned int, float );
extern unsigned int(*AGKCommand287)( unsigned int, unsigned int, float, float, float, float, int );
extern unsigned int(*AGKCommand288)( unsigned int, unsigned int, float, float, int );
extern unsigned int(*AGKCommand289)( unsigned int, unsigned int, float, float, float, float, int );
extern void(*AGKCommand290)( unsigned int, unsigned int, float, int );
extern unsigned int(*AGKCommand291)( float, float, float, float, float, float, float, float );
extern unsigned int(*AGKCommand292)( unsigned int, float, float, float );
extern unsigned int(*AGKCommand293)( unsigned int, unsigned int, float, float, float, float, int );
extern unsigned int(*AGKCommand294)( unsigned int, unsigned int, float, float, int );
extern unsigned int(*AGKCommand295)( unsigned int, unsigned int, float );
extern void(*AGKCommand296)( unsigned int, float, float );
extern void(*AGKCommand297)( unsigned int );
extern void(*AGKCommand298)( unsigned int, float, float );
extern void(*AGKCommand299)( unsigned int );
extern void(*AGKCommand300)( unsigned int, float, float );
extern float(*AGKCommand301)( unsigned int );
extern float(*AGKCommand302)( unsigned int );
extern float(*AGKCommand303)( unsigned int );
extern int(*AGKCommand304)( float, float, float, float );
extern int(*AGKCommand305)( int, float, float, float, float );
extern int(*AGKCommand306)( unsigned int, float, float, float, float );
extern int(*AGKCommand307)( unsigned int, float, float, float, float );
extern int(*AGKCommand308)( float, float, float, float );
extern int(*AGKCommand309)( int, float, float, float, float );
extern int(*AGKCommand310)( unsigned int, float, float, float, float );
extern cSprite*(*AGKCommand311)(  );
extern unsigned int(*AGKCommand312)(  );
extern float(*AGKCommand313)(  );
extern float(*AGKCommand314)(  );
extern float(*AGKCommand315)(  );
extern float(*AGKCommand316)(  );
extern float(*AGKCommand317)(  );
extern int(*AGKCommand318)( unsigned int, float, float, float, float );
extern int(*AGKCommand319)( unsigned int, float, float, float );
extern int(*AGKCommand320)( unsigned int, unsigned int );
extern float(*AGKCommand321)( unsigned int, unsigned int );
extern float(*AGKCommand322)(  );
extern float(*AGKCommand323)(  );
extern float(*AGKCommand324)(  );
extern float(*AGKCommand325)(  );
extern int(*AGKCommand326)(  );
extern int(*AGKCommand327)(  );
extern float(*AGKCommand328)(  );
extern float(*AGKCommand329)(  );
extern unsigned int(*AGKCommand330)(  );
extern unsigned int(*AGKCommand331)(  );
extern int(*AGKCommand332)( unsigned int );
extern int(*AGKCommand333)(  );
extern float(*AGKCommand334)(  );
extern float(*AGKCommand335)(  );
extern unsigned int(*AGKCommand336)(  );
extern int(*AGKCommand337)( unsigned int, unsigned int );
extern float(*AGKCommand338)(  );
extern float(*AGKCommand339)(  );
extern float(*AGKCommand340)(  );
extern float(*AGKCommand341)(  );
extern void(*AGKCommand342)( unsigned int, float, float );
extern unsigned int(*AGKCommand343)( float, float );
extern int(*AGKCommand344)( unsigned int );
extern void(*AGKCommand345)( unsigned int );
extern void(*AGKCommand346)( unsigned int, float, float );
extern void(*AGKCommand347)( unsigned int, int );
extern void(*AGKCommand348)( unsigned int, float );
extern void(*AGKCommand349)( unsigned int, float, float, float, float );
extern void(*AGKCommand350)( unsigned int, float, float );
extern void(*AGKCommand351)( unsigned int, float, float );
extern void(*AGKCommand352)( unsigned int, float );
extern void(*AGKCommand353)( unsigned int, float );
extern void(*AGKCommand354)( unsigned int, float, float );
extern void(*AGKCommand355)( unsigned int, float, float );
extern void(*AGKCommand356)( unsigned int, int );
extern void(*AGKCommand357)( unsigned int, float );
extern void(*AGKCommand358)( unsigned int, float );
extern void(*AGKCommand359)( unsigned int, int );
extern void(*AGKCommand360)( unsigned int );
extern void(*AGKCommand361)( unsigned int, unsigned int );
extern void(*AGKCommand362)( unsigned int, int );
extern void(*AGKCommand363)( unsigned int, int );
extern void(*AGKCommand364)( unsigned int, int );
extern void(*AGKCommand365)( unsigned int, int );
extern float(*AGKCommand366)( unsigned int );
extern float(*AGKCommand367)( unsigned int );
extern int(*AGKCommand368)( unsigned int );
extern int(*AGKCommand369)( unsigned int );
extern int(*AGKCommand370)( unsigned int );
extern float(*AGKCommand371)( unsigned int );
extern float(*AGKCommand372)( unsigned int );
extern float(*AGKCommand373)( unsigned int );
extern float(*AGKCommand374)( unsigned int );
extern float(*AGKCommand375)( unsigned int );
extern float(*AGKCommand376)( unsigned int );
extern float(*AGKCommand377)( unsigned int );
extern int(*AGKCommand378)( unsigned int );
extern void(*AGKCommand379)( unsigned int, float, float, float, float );
extern void(*AGKCommand380)( unsigned int );
extern void(*AGKCommand381)( unsigned int, float, unsigned int, unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand382)( unsigned int );
extern void(*AGKCommand383)( unsigned int, float, float );
extern void(*AGKCommand384)( unsigned int );
extern void(*AGKCommand385)( unsigned int, int );
extern void(*AGKCommand386)( unsigned int, float );
extern void(*AGKCommand387)( unsigned int, float, float );
extern void(*AGKCommand388)( unsigned int, const char * );
extern unsigned int(*AGKCommand389)( const char * );
extern unsigned int(*AGKCommand390)( unsigned int );
extern void(*AGKCommand391)( unsigned int );
extern void(*AGKCommand392)(  );
extern void(*AGKCommand393)( unsigned int, const char * );
extern void(*AGKCommand394)( unsigned int, float, float );
extern void(*AGKCommand395)( unsigned int, float );
extern void(*AGKCommand396)( unsigned int, float );
extern void(*AGKCommand397)( unsigned int, float );
extern void(*AGKCommand398)( unsigned int, float );
extern void(*AGKCommand399)( unsigned int, int );
extern void(*AGKCommand400)( unsigned int, float );
extern void(*AGKCommand401)( unsigned int, float );
extern void(*AGKCommand402)( unsigned int, float );
extern void(*AGKCommand403)( unsigned int, int );
extern void(*AGKCommand404)( unsigned int, int );
extern void(*AGKCommand405)( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand406)( unsigned int, unsigned int );
extern void(*AGKCommand407)( unsigned int, unsigned int );
extern void(*AGKCommand408)( unsigned int, unsigned int );
extern void(*AGKCommand409)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand410)( unsigned int );
extern unsigned int(*AGKCommand411)( unsigned int );
extern unsigned int(*AGKCommand412)( unsigned int );
extern unsigned int(*AGKCommand413)( unsigned int );
extern void(*AGKCommand414)( unsigned int, unsigned int, float, float );
extern void(*AGKCommand415)( unsigned int, unsigned int, float );
extern void(*AGKCommand416)( unsigned int, unsigned int, float );
extern void(*AGKCommand417)( unsigned int, unsigned int, float );
extern void(*AGKCommand418)( unsigned int, unsigned int, float );
extern void(*AGKCommand419)( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand420)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand421)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand422)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand423)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand424)( unsigned int, unsigned int, unsigned int );
extern float(*AGKCommand425)( unsigned int, unsigned int );
extern float(*AGKCommand426)( unsigned int, unsigned int );
extern float(*AGKCommand427)( unsigned int, unsigned int );
extern float(*AGKCommand428)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand429)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand430)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand431)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand432)( unsigned int, unsigned int );
extern float(*AGKCommand433)( unsigned int );
extern float(*AGKCommand434)( unsigned int );
extern unsigned int(*AGKCommand435)( unsigned int );
extern float(*AGKCommand436)( unsigned int );
extern float(*AGKCommand437)( unsigned int );
extern int(*AGKCommand438)( unsigned int, float, float );
extern void(*AGKCommand439)( unsigned int, unsigned int );
extern void(*AGKCommand440)( unsigned int, unsigned int );
extern void(*AGKCommand441)( unsigned int, unsigned int );
extern void(*AGKCommand442)( unsigned int );
extern void(*AGKCommand443)( unsigned int );
extern void(*AGKCommand444)( unsigned int );
extern void(*AGKCommand445)( unsigned int );
extern void(*AGKCommand446)( unsigned int, int );
extern void(*AGKCommand447)( unsigned int, float );
extern void(*AGKCommand448)( unsigned int, float, float, float, float );
extern void(*AGKCommand449)( unsigned int, int );
extern void(*AGKCommand450)( unsigned int, unsigned int );
extern int(*AGKCommand451)( unsigned int );
extern int(*AGKCommand452)( unsigned int );
extern float(*AGKCommand453)( unsigned int );
extern float(*AGKCommand454)( unsigned int );
extern float(*AGKCommand455)( unsigned int );
extern char*(*AGKCommand456)( unsigned int );
extern unsigned int(*AGKCommand457)( const char * );
extern void(*AGKCommand458)( unsigned int, const char * );
extern int(*AGKCommand459)( unsigned int );
extern int(*AGKCommand460)( const char * );
extern void(*AGKCommand461)( unsigned int );
extern void(*AGKCommand462)( const char * );
extern void(*AGKCommand463)( int );
extern void(*AGKCommand464)( float );
extern void(*AGKCommand465)( const char * );
extern void(*AGKCommand466)( int );
extern void(*AGKCommand467)( float );
extern void(*AGKCommand468)( float );
extern void(*AGKCommand469)( float );
extern void(*AGKCommand470)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand471)( unsigned int, unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand472)( int );
extern void(*AGKCommand473)( unsigned int );
extern unsigned int(*AGKCommand474)(  );
extern int(*AGKCommand475)( unsigned int );
extern void(*AGKCommand476)( unsigned int );
extern void(*AGKCommand477)( unsigned int, const char*, float, unsigned int, int );
extern unsigned int(*AGKCommand478)( const char*, float, unsigned int, int );
extern void(*AGKCommand479)( unsigned int, const char*, float, unsigned int );
extern unsigned int(*AGKCommand480)( const char*, float, unsigned int );
extern void(*AGKCommand481)( unsigned int, float, float );
extern void(*AGKCommand482)( unsigned int, float );
extern void(*AGKCommand483)( unsigned int, int, int );
extern void(*AGKCommand484)( unsigned int, unsigned int );
extern void(*AGKCommand485)( unsigned int, int );
extern void(*AGKCommand486)( unsigned int, int );
extern float(*AGKCommand487)( unsigned int );
extern float(*AGKCommand488)( unsigned int );
extern float(*AGKCommand489)( unsigned int );
extern int(*AGKCommand490)( unsigned int, const char* );
extern int(*AGKCommand491)( unsigned int, int );
extern float(*AGKCommand492)( unsigned int, int );
extern float(*AGKCommand493)( unsigned int, int );
extern float(*AGKCommand494)( unsigned int, int );
extern float(*AGKCommand495)( unsigned int, int );
extern float(*AGKCommand496)( unsigned int, int );
extern float(*AGKCommand497)( unsigned int, int );
extern void(*AGKCommand498)( unsigned int, int, float, float );
extern void(*AGKCommand499)( unsigned int, int, float );
extern void(*AGKCommand500)( unsigned int, int, float, float );
extern void(*AGKCommand501)( unsigned int, int, int );
extern void(*AGKCommand502)( unsigned int, const char*, float, int, float );
extern void(*AGKCommand503)( unsigned int, const char*, float, float );
extern void(*AGKCommand504)( unsigned int, float );
extern float(*AGKCommand505)( unsigned int );
extern void(*AGKCommand506)( unsigned int );
extern int(*AGKCommand507)( unsigned int );
extern int(*AGKCommand508)( unsigned int );
extern float(*AGKCommand509)( unsigned int, const char* );
extern int(*AGKCommand510)(  );
extern int(*AGKCommand511)(  );
extern int(*AGKCommand512)(  );
extern int(*AGKCommand513)(  );
extern int(*AGKCommand514)(  );
extern int(*AGKCommand515)(  );
extern int(*AGKCommand516)(  );
extern int(*AGKCommand517)(  );
extern int(*AGKCommand518)(  );
extern void(*AGKCommand519)( unsigned int );
extern int(*AGKCommand520)( unsigned int );
extern void(*AGKCommand521)( unsigned int, float );
extern void(*AGKCommand522)( unsigned int, float );
extern unsigned int(*AGKCommand523)( float );
extern int(*AGKCommand524)( unsigned int );
extern void(*AGKCommand525)( unsigned int, float, float, int );
extern void(*AGKCommand526)( unsigned int, float, float, int );
extern void(*AGKCommand527)( unsigned int, float, float, int );
extern void(*AGKCommand528)( unsigned int, float, float, int );
extern void(*AGKCommand529)( unsigned int, int, int, int );
extern void(*AGKCommand530)( unsigned int, int, int, int );
extern void(*AGKCommand531)( unsigned int, int, int, int );
extern void(*AGKCommand532)( unsigned int, int, int, int );
extern float(*AGKCommand533)( unsigned int );
extern float(*AGKCommand534)( unsigned int );
extern float(*AGKCommand535)( unsigned int );
extern float(*AGKCommand536)( unsigned int );
extern int(*AGKCommand537)( unsigned int );
extern int(*AGKCommand538)( unsigned int );
extern int(*AGKCommand539)( unsigned int );
extern int(*AGKCommand540)( unsigned int );
extern void(*AGKCommand541)( unsigned int, float );
extern void(*AGKCommand542)( unsigned int );
extern int(*AGKCommand543)( unsigned int );
extern void(*AGKCommand544)( unsigned int, float );
extern unsigned int(*AGKCommand545)( float );
extern int(*AGKCommand546)( unsigned int );
extern void(*AGKCommand547)( unsigned int, float, float, int );
extern void(*AGKCommand548)( unsigned int, float, float, int );
extern void(*AGKCommand549)( unsigned int, float, float, int );
extern void(*AGKCommand550)( unsigned int, float, float, int );
extern void(*AGKCommand551)( unsigned int, float, float, int );
extern void(*AGKCommand552)( unsigned int, float, float, int );
extern void(*AGKCommand553)( unsigned int, float, float, int );
extern void(*AGKCommand554)( unsigned int, int, int, int );
extern void(*AGKCommand555)( unsigned int, int, int, int );
extern void(*AGKCommand556)( unsigned int, int, int, int );
extern void(*AGKCommand557)( unsigned int, int, int, int );
extern void(*AGKCommand558)( unsigned int, unsigned int, float );
extern void(*AGKCommand559)( unsigned int, unsigned int );
extern int(*AGKCommand560)( unsigned int, unsigned int );
extern void(*AGKCommand561)( unsigned int, float );
extern unsigned int(*AGKCommand562)( float );
extern int(*AGKCommand563)( unsigned int );
extern void(*AGKCommand564)( unsigned int, float, float, int );
extern void(*AGKCommand565)( unsigned int, float, float, int );
extern void(*AGKCommand566)( unsigned int, float, float, int );
extern void(*AGKCommand567)( unsigned int, float, float, int );
extern void(*AGKCommand568)( unsigned int, float, float, int );
extern void(*AGKCommand569)( unsigned int, float, float, int );
extern void(*AGKCommand570)( unsigned int, int, int, int );
extern void(*AGKCommand571)( unsigned int, int, int, int );
extern void(*AGKCommand572)( unsigned int, int, int, int );
extern void(*AGKCommand573)( unsigned int, int, int, int );
extern void(*AGKCommand574)( unsigned int, unsigned int, float );
extern void(*AGKCommand575)( unsigned int, unsigned int );
extern int(*AGKCommand576)( unsigned int, unsigned int );
extern void(*AGKCommand577)( unsigned int, float );
extern unsigned int(*AGKCommand578)( float );
extern int(*AGKCommand579)( unsigned int );
extern void(*AGKCommand580)( unsigned int, float, float, int );
extern void(*AGKCommand581)( unsigned int, float, float, int );
extern void(*AGKCommand582)( unsigned int, float, float, int );
extern void(*AGKCommand583)( unsigned int, int, int, int );
extern void(*AGKCommand584)( unsigned int, int, int, int );
extern void(*AGKCommand585)( unsigned int, int, int, int );
extern void(*AGKCommand586)( unsigned int, int, int, int );
extern void(*AGKCommand587)( unsigned int, unsigned int, unsigned int, float );
extern void(*AGKCommand588)( unsigned int, unsigned int, unsigned int );
extern int(*AGKCommand589)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand590)( unsigned int, float );
extern unsigned int(*AGKCommand591)( float );
extern int(*AGKCommand592)( unsigned int );
extern void(*AGKCommand593)( unsigned int, float, float, int );
extern void(*AGKCommand594)( unsigned int, float, float, int );
extern void(*AGKCommand595)( unsigned int, float, float, int );
extern void(*AGKCommand596)( unsigned int, float, float, int );
extern void(*AGKCommand597)( unsigned int, float, float, int );
extern void(*AGKCommand598)( unsigned int, float, float, int );
extern void(*AGKCommand599)( unsigned int, float, float, int );
extern void(*AGKCommand600)( unsigned int, float, float, int );
extern void(*AGKCommand601)( unsigned int, float, float, int );
extern void(*AGKCommand602)( unsigned int, int, int, int );
extern void(*AGKCommand603)( unsigned int, int, int, int );
extern void(*AGKCommand604)( unsigned int, int, int, int );
extern void(*AGKCommand605)( unsigned int, int, int, int );
extern void(*AGKCommand606)( unsigned int, unsigned int, float );
extern void(*AGKCommand607)( unsigned int, unsigned int );
extern int(*AGKCommand608)( unsigned int, unsigned int );
extern void(*AGKCommand609)( unsigned int, float );
extern unsigned int(*AGKCommand610)( float );
extern int(*AGKCommand611)( unsigned int );
extern void(*AGKCommand612)( unsigned int, float, float, int );
extern void(*AGKCommand613)( unsigned int, float, float, int );
extern void(*AGKCommand614)( unsigned int, float, float, int );
extern void(*AGKCommand615)( unsigned int, float, float, int );
extern void(*AGKCommand616)( unsigned int, float, float, int );
extern void(*AGKCommand617)( unsigned int, float, float, int );
extern void(*AGKCommand618)( unsigned int, float, float, int );
extern void(*AGKCommand619)( unsigned int, unsigned int, float );
extern void(*AGKCommand620)( unsigned int, unsigned int );
extern int(*AGKCommand621)( unsigned int, unsigned int );
extern void(*AGKCommand622)( unsigned int );
extern unsigned int(*AGKCommand623)(  );
extern void(*AGKCommand624)( unsigned int );
extern void(*AGKCommand625)( unsigned int );
extern void(*AGKCommand626)( unsigned int, unsigned int, float );
extern void(*AGKCommand627)( unsigned int, unsigned int, unsigned int, float );
extern void(*AGKCommand628)( unsigned int, unsigned int, unsigned int, float );
extern void(*AGKCommand629)( unsigned int, unsigned int, unsigned int, unsigned int, float );
extern void(*AGKCommand630)( unsigned int, unsigned int, unsigned int, float );
extern void(*AGKCommand631)( unsigned int, unsigned int, unsigned int, float );
extern void(*AGKCommand632)( unsigned int );
extern void(*AGKCommand633)( unsigned int );
extern void(*AGKCommand634)( unsigned int );
extern int(*AGKCommand635)( unsigned int );
extern void(*AGKCommand636)( unsigned int, float );
extern float(*AGKCommand637)( unsigned int );
extern void(*AGKCommand638)( float );
extern void(*AGKCommand639)( unsigned int, float );
extern void(*AGKCommand640)( unsigned int, unsigned int, float );
extern void(*AGKCommand641)( unsigned int, unsigned int, float );
extern void(*AGKCommand642)( unsigned int, unsigned int, unsigned int, float );
extern void(*AGKCommand643)( unsigned int, unsigned int, float );
extern void(*AGKCommand644)( unsigned int, unsigned int, float );
extern void(*AGKCommand645)( unsigned int, float );
extern unsigned int(*AGKCommand646)( int );
extern unsigned int(*AGKCommand647)( int );
extern unsigned int(*AGKCommand648)(  );
extern int(*AGKCommand649)( unsigned int );
extern float(*AGKCommand650)( unsigned int );
extern float(*AGKCommand651)( unsigned int );
extern float(*AGKCommand652)( unsigned int );
extern float(*AGKCommand653)( unsigned int );
extern float(*AGKCommand654)( unsigned int );
extern float(*AGKCommand655)( unsigned int );
extern int(*AGKCommand656)( unsigned int );
extern void(*AGKCommand657)( unsigned int, int );
extern int(*AGKCommand658)( unsigned int );
extern float(*AGKCommand659)( unsigned int );
extern void(*AGKCommand660)( unsigned int, const char* );
extern unsigned int(*AGKCommand661)( const char* );
extern void(*AGKCommand662)( unsigned int, const char* );
extern unsigned int(*AGKCommand663)( const char* );
extern void(*AGKCommand664)( unsigned int, const char* );
extern unsigned int(*AGKCommand665)( unsigned int );
extern unsigned int(*AGKCommand666)( unsigned int, int );
extern unsigned int(*AGKCommand667)( unsigned int, int, int );
extern unsigned int(*AGKCommand668)( unsigned int, int, int, int );
extern void(*AGKCommand669)( unsigned int );
extern void(*AGKCommand670)( unsigned int );
extern unsigned int(*AGKCommand671)( unsigned int );
extern unsigned int(*AGKCommand672)( unsigned int );
extern unsigned int(*AGKCommand673)( unsigned int );
extern void(*AGKCommand674)( int );
extern float(*AGKCommand675)(  );
extern float(*AGKCommand676)(  );
extern int(*AGKCommand677)( unsigned int );
extern int(*AGKCommand678)( unsigned int );
extern float(*AGKCommand679)( unsigned int );
extern void(*AGKCommand680)( unsigned int, int );
extern void(*AGKCommand681)( unsigned int, float );
extern void(*AGKCommand682)( unsigned int, float );
extern int(*AGKCommand683)( unsigned int );
extern void(*AGKCommand684)( unsigned int );
extern unsigned int(*AGKCommand685)( const char * );
extern void(*AGKCommand686)( unsigned int, const char * );
extern void(*AGKCommand687)( unsigned int, int );
extern void(*AGKCommand688)(  );
extern void(*AGKCommand689)( unsigned int );
extern void(*AGKCommand690)( unsigned int, int );
extern void(*AGKCommand691)( unsigned int, int, unsigned int, unsigned int );
extern void(*AGKCommand692)(  );
extern void(*AGKCommand693)(  );
extern void(*AGKCommand694)(  );
extern void(*AGKCommand695)( unsigned int );
extern int(*AGKCommand696)(  );
extern void(*AGKCommand697)( int );
extern unsigned int(*AGKCommand698)( unsigned int );
extern float(*AGKCommand699)( unsigned int );
extern float(*AGKCommand700)(  );
extern void(*AGKCommand701)( float, int );
extern unsigned int(*AGKCommand702)( const char* );
extern void(*AGKCommand703)( unsigned int, const char* );
extern int(*AGKCommand704)( unsigned int );
extern void(*AGKCommand705)( unsigned int, int );
extern void(*AGKCommand706)( unsigned int );
extern void(*AGKCommand707)( unsigned int, int );
extern void(*AGKCommand708)( unsigned int );
extern void(*AGKCommand709)( unsigned int );
extern void(*AGKCommand710)( unsigned int );
extern void(*AGKCommand711)( unsigned int );
extern int(*AGKCommand712)( unsigned int );
extern int(*AGKCommand713)( unsigned int );
extern float(*AGKCommand714)( unsigned int );
extern float(*AGKCommand715)( unsigned int );
extern void(*AGKCommand716)( unsigned int, float, int );
extern void(*AGKCommand717)( int );
extern void(*AGKCommand718)( unsigned int, float, float );
extern void(*AGKCommand719)( unsigned int, int );
extern void(*AGKCommand720)( const char* );
extern void(*AGKCommand721)(  );
extern int(*AGKCommand722)(  );
extern void(*AGKCommand723)( const char * );
extern int(*AGKCommand724)( const char * );
extern char*(*AGKCommand725)( const char* );
extern char*(*AGKCommand726)( const char*, int );
extern char*(*AGKCommand727)( const char * );
extern char*(*AGKCommand728)( const char *, const char * );
extern int(*AGKCommand729)( const char * );
extern int(*AGKCommand730)(  );
extern char*(*AGKCommand731)( int );
extern unsigned int(*AGKCommand732)( const char * );
extern void(*AGKCommand733)( unsigned int );
extern int(*AGKCommand734)( unsigned int );
extern int(*AGKCommand735)( unsigned int );
extern char*(*AGKCommand736)( unsigned int, int );
extern char*(*AGKCommand737)( unsigned int, int );
extern void(*AGKCommand738)( unsigned int, const char *, int );
extern void(*AGKCommand739)( unsigned int, const char * );
extern unsigned int(*AGKCommand740)( const char *, int );
extern unsigned int(*AGKCommand741)( const char * );
extern unsigned int(*AGKCommand742)( const char * );
extern void(*AGKCommand743)( unsigned int, const char * );
extern int(*AGKCommand744)( unsigned int );
extern void(*AGKCommand745)( unsigned int );
extern int(*AGKCommand746)( unsigned int );
extern int(*AGKCommand747)( unsigned int );
extern int(*AGKCommand748)( unsigned int );
extern void(*AGKCommand749)( unsigned int, int );
extern void(*AGKCommand750)( unsigned int, int );
extern void(*AGKCommand751)( unsigned int, int );
extern void(*AGKCommand752)( unsigned int, float );
extern void(*AGKCommand753)( unsigned int, const char* );
extern void(*AGKCommand754)( unsigned int, const char* );
extern void(*AGKCommand755)( unsigned int, const char* );
extern int(*AGKCommand756)( unsigned int );
extern int(*AGKCommand757)( unsigned int );
extern float(*AGKCommand758)( unsigned int );
extern char*(*AGKCommand759)( unsigned int );
extern char*(*AGKCommand760)( unsigned int );
extern char*(*AGKCommand761)( unsigned int );
extern char*(*AGKCommand762)( int );
extern char*(*AGKCommand763)( float );
extern char*(*AGKCommand764)( float, unsigned int );
extern char*(*AGKCommand765)( int );
extern char*(*AGKCommand766)( int );
extern char*(*AGKCommand767)( const char* );
extern char*(*AGKCommand768)( const char* );
extern int(*AGKCommand769)( const char* );
extern int(*AGKCommand770)( const char*, int );
extern float(*AGKCommand771)( const char* );
extern char*(*AGKCommand772)( const char*, unsigned int );
extern char*(*AGKCommand773)( const char*, unsigned int );
extern char*(*AGKCommand774)( const char*, unsigned int, int );
extern unsigned int(*AGKCommand775)( const char* );
extern unsigned int(*AGKCommand776)( const char* );
extern unsigned int(*AGKCommand777)( const char* );
extern char*(*AGKCommand778)( unsigned int );
extern char*(*AGKCommand779)( const char* );
extern char*(*AGKCommand780)( const char* );
extern char*(*AGKCommand781)( unsigned int );
extern int(*AGKCommand782)( const char*, const char* );
extern int(*AGKCommand783)( const char*, const char*, int, int );
extern int(*AGKCommand784)( const char*, const char* );
extern int(*AGKCommand785)( const char*, const char*, int, int );
extern int(*AGKCommand786)( const char*, const char* );
extern int(*AGKCommand787)( const char*, const char*, int, int );
extern int(*AGKCommand788)( const char*, const char* );
extern int(*AGKCommand789)( const char*, const char*, int, int );
extern char*(*AGKCommand790)( const char*, const char*, const char*, int );
extern char*(*AGKCommand791)( const char*, const char* );
extern char*(*AGKCommand792)( const char*, const char* );
extern char*(*AGKCommand793)( const char*, const char* );
extern int(*AGKCommand794)( const char*, const char* );
extern char*(*AGKCommand795)( const char*, const char*, int );
extern int(*AGKCommand796)( const char*, const char* );
extern char*(*AGKCommand797)( const char*, const char*, int );
extern char*(*AGKCommand798)(  );
extern char*(*AGKCommand799)(  );
extern int(*AGKCommand800)( const char* );
extern char*(*AGKCommand801)(  );
extern char*(*AGKCommand802)( int );
extern char*(*AGKCommand803)(  );
extern int(*AGKCommand804)(  );
extern int(*AGKCommand805)( int );
extern char*(*AGKCommand806)(  );
extern char*(*AGKCommand807)( int );
extern char*(*AGKCommand808)(  );
extern int(*AGKCommand809)(  );
extern int(*AGKCommand810)( int );
extern unsigned int(*AGKCommand811)( unsigned int );
extern unsigned int(*AGKCommand812)( const char *, unsigned int );
extern unsigned int(*AGKCommand813)( unsigned int );
extern void(*AGKCommand814)( unsigned int );
extern unsigned int(*AGKCommand815)( const char *, int, int );
extern int(*AGKCommand816)( unsigned int, const char *, int, int );
extern int(*AGKCommand817)( unsigned int );
extern int(*AGKCommand818)( unsigned int );
extern void(*AGKCommand819)( unsigned int );
extern char*(*AGKCommand820)( unsigned int );
extern int(*AGKCommand821)( unsigned int );
extern int(*AGKCommand822)( unsigned int );
extern int(*AGKCommand823)( unsigned int );
extern int(*AGKCommand824)( unsigned int );
extern float(*AGKCommand825)( unsigned int );
extern char*(*AGKCommand826)( unsigned int );
extern int(*AGKCommand827)( unsigned int, int );
extern int(*AGKCommand828)( unsigned int, int );
extern int(*AGKCommand829)( unsigned int, float );
extern int(*AGKCommand830)( unsigned int, const char * );
extern unsigned int(*AGKCommand831)( const char *, int );
extern int(*AGKCommand832)( unsigned int, const char *, int );
extern unsigned int(*AGKCommand833)( unsigned int );
extern void(*AGKCommand834)( unsigned int );
extern unsigned int(*AGKCommand835)( const char *, const char *, int );
extern unsigned int(*AGKCommand836)( const char *, const char *, int, int );
extern void(*AGKCommand837)( unsigned int );
extern unsigned int(*AGKCommand838)( const char *, const char * );
extern unsigned int(*AGKCommand839)( const char *, unsigned int, const char * );
extern unsigned int(*AGKCommand840)( unsigned int );
extern char*(*AGKCommand841)(  );
extern char*(*AGKCommand842)(  );
extern void(*AGKCommand843)( unsigned int );
extern void(*AGKCommand844)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand845)( unsigned int );
extern unsigned int(*AGKCommand846)( unsigned int );
extern unsigned int(*AGKCommand847)( unsigned int );
extern unsigned int(*AGKCommand848)( unsigned int );
extern unsigned int(*AGKCommand849)( unsigned int, unsigned int );
extern void(*AGKCommand850)( unsigned int, unsigned int );
extern char*(*AGKCommand851)( unsigned int, unsigned int );
extern float(*AGKCommand852)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand853)( unsigned int );
extern void(*AGKCommand854)( unsigned int, const char *, int );
extern void(*AGKCommand855)( unsigned int, const char *, int, int );
extern void(*AGKCommand856)( unsigned int, const char *, float );
extern void(*AGKCommand857)( unsigned int, const char *, float, int );
extern int(*AGKCommand858)( unsigned int, unsigned int, const char * );
extern float(*AGKCommand859)( unsigned int, unsigned int, const char * );
extern unsigned int(*AGKCommand860)(  );
extern void(*AGKCommand861)( unsigned int, int );
extern void(*AGKCommand862)( unsigned int, float );
extern void(*AGKCommand863)( unsigned int, const char * );
extern char*(*AGKCommand864)( unsigned int );
extern unsigned int(*AGKCommand865)( unsigned int );
extern int(*AGKCommand866)( unsigned int );
extern float(*AGKCommand867)( unsigned int );
extern char*(*AGKCommand868)( unsigned int );
extern void(*AGKCommand869)( unsigned int );
extern void(*AGKCommand870)( unsigned int, unsigned int, unsigned int );
extern unsigned int(*AGKCommand871)( unsigned int );
extern void(*AGKCommand872)( unsigned int, unsigned int, unsigned int, int );
extern int(*AGKCommand873)( unsigned int, unsigned int, unsigned int );
extern unsigned int(*AGKCommand874)(  );
extern void(*AGKCommand875)( unsigned int );
extern unsigned int(*AGKCommand876)( unsigned int, const char *, int );
extern unsigned int(*AGKCommand877)( unsigned int, const char *, int, const char *, const char * );
extern void(*AGKCommand878)( unsigned int );
extern void(*AGKCommand879)( unsigned int, int );
extern void(*AGKCommand880)( unsigned int, int );
extern char*(*AGKCommand881)( unsigned int, const char * );
extern char*(*AGKCommand882)( unsigned int, const char *, const char * );
extern int(*AGKCommand883)( unsigned int, const char * );
extern int(*AGKCommand884)( unsigned int, const char *, const char * );
extern int(*AGKCommand885)( unsigned int, const char *, const char *, const char * );
extern char*(*AGKCommand886)( unsigned int );
extern int(*AGKCommand887)( unsigned int );
extern int(*AGKCommand888)( unsigned int, const char *, const char * );
extern int(*AGKCommand889)( unsigned int, const char *, const char *, const char * );
extern int(*AGKCommand890)( unsigned int );
extern float(*AGKCommand891)( unsigned int );
extern void(*AGKCommand892)( int );
extern char*(*AGKCommand893)(  );
extern int(*AGKCommand894)(  );
extern void(*AGKCommand895)( const char * );
extern void(*AGKCommand896)( const char* );
extern int(*AGKCommand897)(  );
extern float(*AGKCommand898)(  );
extern float(*AGKCommand899)(  );
extern float(*AGKCommand900)(  );
extern float(*AGKCommand901)(  );
extern int(*AGKCommand902)(  );
extern int(*AGKCommand903)(  );
extern int(*AGKCommand904)(  );
extern int(*AGKCommand905)(  );
extern int(*AGKCommand906)(  );
extern int(*AGKCommand907)(  );
extern int(*AGKCommand908)(  );
extern int(*AGKCommand909)(  );
extern int(*AGKCommand910)(  );
extern int(*AGKCommand911)(  );
extern int(*AGKCommand912)(  );
extern int(*AGKCommand913)(  );
extern int(*AGKCommand914)(  );
extern int(*AGKCommand915)(  );
extern int(*AGKCommand916)(  );
extern float(*AGKCommand917)(  );
extern float(*AGKCommand918)(  );
extern float(*AGKCommand919)(  );
extern float(*AGKCommand920)(  );
extern float(*AGKCommand921)(  );
extern float(*AGKCommand922)(  );
extern float(*AGKCommand923)(  );
extern float(*AGKCommand924)(  );
extern float(*AGKCommand925)(  );
extern float(*AGKCommand926)(  );
extern float(*AGKCommand927)(  );
extern float(*AGKCommand928)(  );
extern float(*AGKCommand929)(  );
extern float(*AGKCommand930)(  );
extern float(*AGKCommand931)(  );
extern float(*AGKCommand932)(  );
extern float(*AGKCommand933)(  );
extern float(*AGKCommand934)(  );
extern float(*AGKCommand935)(  );
extern void(*AGKCommand936)(  );
extern int(*AGKCommand937)( unsigned int );
extern int(*AGKCommand938)( unsigned int );
extern float(*AGKCommand939)( unsigned int );
extern float(*AGKCommand940)( unsigned int );
extern float(*AGKCommand941)( unsigned int );
extern float(*AGKCommand942)( unsigned int );
extern float(*AGKCommand943)( unsigned int );
extern float(*AGKCommand944)( unsigned int );
extern int(*AGKCommand945)( unsigned int, unsigned int );
extern int(*AGKCommand946)( unsigned int, unsigned int );
extern int(*AGKCommand947)( unsigned int, unsigned int );
extern void(*AGKCommand948)( float );
extern void(*AGKCommand949)( unsigned int, float, float, float );
extern void(*AGKCommand950)( unsigned int );
extern int(*AGKCommand951)( unsigned int );
extern float(*AGKCommand952)( unsigned int );
extern float(*AGKCommand953)( unsigned int );
extern void(*AGKCommand954)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand955)( unsigned int, float, float );
extern void(*AGKCommand956)( unsigned int, float );
extern void(*AGKCommand957)( unsigned int, int );
extern void(*AGKCommand958)( unsigned int, int );
extern void(*AGKCommand959)( unsigned int, unsigned int );
extern void(*AGKCommand960)( unsigned int, unsigned int );
extern void(*AGKCommand961)( float );
extern void(*AGKCommand962)( unsigned int, float, float, float );
extern void(*AGKCommand963)( unsigned int );
extern int(*AGKCommand964)( unsigned int );
extern int(*AGKCommand965)( unsigned int );
extern int(*AGKCommand966)( unsigned int );
extern int(*AGKCommand967)( unsigned int );
extern void(*AGKCommand968)( unsigned int, unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand969)( unsigned int, unsigned int );
extern void(*AGKCommand970)( unsigned int, float, float );
extern void(*AGKCommand971)( unsigned int, float );
extern void(*AGKCommand972)( unsigned int, float, float );
extern void(*AGKCommand973)( unsigned int, int );
extern void(*AGKCommand974)( unsigned int, int );
extern void(*AGKCommand975)( unsigned int, unsigned int );
extern void(*AGKCommand976)( unsigned int, unsigned int );
extern void(*AGKCommand977)( unsigned int, const char * );
extern int(*AGKCommand978)( unsigned int );
extern int(*AGKCommand979)( unsigned int );
extern int(*AGKCommand980)( unsigned int );
extern int(*AGKCommand981)(  );
extern float(*AGKCommand982)(  );
extern float(*AGKCommand983)(  );
extern float(*AGKCommand984)(  );
extern float(*AGKCommand985)(  );
extern int(*AGKCommand986)(  );
extern int(*AGKCommand987)(  );
extern int(*AGKCommand988)(  );
extern float(*AGKCommand989)(  );
extern float(*AGKCommand990)(  );
extern float(*AGKCommand991)(  );
extern float(*AGKCommand992)(  );
extern void(*AGKCommand993)( float );
extern void(*AGKCommand994)( float, float, float );
extern int(*AGKCommand995)( unsigned int );
extern int(*AGKCommand996)( unsigned int );
extern int(*AGKCommand997)( unsigned int );
extern void(*AGKCommand998)( unsigned int, float, float, float );
extern void(*AGKCommand999)(  );
extern void(*AGKCommand1000)( const char* );
extern void(*AGKCommand1001)(  );
extern int(*AGKCommand1002)(  );
extern int(*AGKCommand1003)(  );
extern int(*AGKCommand1004)(  );
extern char*(*AGKCommand1005)(  );
extern unsigned int(*AGKCommand1006)(  );
extern void(*AGKCommand1007)( float );
extern void(*AGKCommand1008)( int );
extern void(*AGKCommand1009)( unsigned int );
extern unsigned int(*AGKCommand1010)(  );
extern int(*AGKCommand1011)( unsigned int );
extern void(*AGKCommand1012)( unsigned int );
extern int(*AGKCommand1013)( unsigned int );
extern unsigned int(*AGKCommand1014)(  );
extern void(*AGKCommand1015)( unsigned int, float, float );
extern void(*AGKCommand1016)( unsigned int, float, float );
extern void(*AGKCommand1017)( unsigned int, int );
extern void(*AGKCommand1018)( unsigned int, float );
extern void(*AGKCommand1019)( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1020)( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1021)( unsigned int, const char * );
extern void(*AGKCommand1022)( unsigned int, unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1023)( unsigned int, unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1024)( unsigned int, unsigned int );
extern void(*AGKCommand1025)( unsigned int, unsigned int );
extern void(*AGKCommand1026)( unsigned int, unsigned int );
extern void(*AGKCommand1027)( unsigned int, float );
extern void(*AGKCommand1028)( unsigned int, int );
extern void(*AGKCommand1029)( unsigned int, int );
extern void(*AGKCommand1030)( unsigned int, int );
extern void(*AGKCommand1031)( unsigned int, int );
extern void(*AGKCommand1032)( unsigned int, unsigned int );
extern void(*AGKCommand1033)( unsigned int, unsigned int );
extern void(*AGKCommand1034)( unsigned int, float );
extern void(*AGKCommand1035)( unsigned int, float );
extern void(*AGKCommand1036)( unsigned int, unsigned int );
extern void(*AGKCommand1037)( unsigned int, unsigned int );
extern void(*AGKCommand1038)( unsigned int, int );
extern void(*AGKCommand1039)( unsigned int, float, float, float, float );
extern void(*AGKCommand1040)( unsigned int, int );
extern void(*AGKCommand1041)( unsigned int, int );
extern void(*AGKCommand1042)( unsigned int, int );
extern void(*AGKCommand1043)( unsigned int, int );
extern char*(*AGKCommand1044)( unsigned int );
extern float(*AGKCommand1045)( unsigned int );
extern float(*AGKCommand1046)( unsigned int );
extern float(*AGKCommand1047)( unsigned int );
extern float(*AGKCommand1048)( unsigned int );
extern int(*AGKCommand1049)( unsigned int );
extern int(*AGKCommand1050)( unsigned int );
extern int(*AGKCommand1051)( unsigned int );
extern int(*AGKCommand1052)( unsigned int );
extern int(*AGKCommand1053)( unsigned int );
extern unsigned int(*AGKCommand1054)(  );
extern unsigned int(*AGKCommand1055)(  );
extern unsigned int(*AGKCommand1056)(  );
extern unsigned int(*AGKCommand1057)(  );
extern unsigned int(*AGKCommand1058)(  );
extern unsigned int(*AGKCommand1059)(  );
extern unsigned int(*AGKCommand1060)(  );
extern float(*AGKCommand1061)(  );
extern float(*AGKCommand1062)(  );
extern float(*AGKCommand1063)(  );
extern float(*AGKCommand1064)(  );
extern unsigned int(*AGKCommand1065)(  );
extern unsigned int(*AGKCommand1066)(  );
extern char*(*AGKCommand1067)( unsigned int );
extern float(*AGKCommand1068)(  );
extern int(*AGKCommand1069)( int );
extern int(*AGKCommand1070)( int, int, int, int, int, int );
extern int(*AGKCommand1071)( int );
extern int(*AGKCommand1072)( int );
extern int(*AGKCommand1073)( int );
extern int(*AGKCommand1074)( int );
extern int(*AGKCommand1075)( int );
extern int(*AGKCommand1076)( int );
extern void(*AGKCommand1077)( const char* );
extern void(*AGKCommand1078)( const char* );
extern void(*AGKCommand1079)( const char* );
extern void(*AGKCommand1080)( const char*, const char* );
extern void(*AGKCommand1081)( const char* );
extern void(*AGKCommand1082)( int );
extern void(*AGKCommand1083)( int );
extern void(*AGKCommand1084)(  );
extern void(*AGKCommand1085)(  );
extern void(*AGKCommand1086)(  );
extern int(*AGKCommand1087)(  );
extern int(*AGKCommand1088)(  );
extern int(*AGKCommand1089)(  );
extern void(*AGKCommand1090)(  );
extern int(*AGKCommand1091)(  );
extern int(*AGKCommand1092)(  );
extern void(*AGKCommand1093)(  );
extern void(*AGKCommand1094)(  );
extern void(*AGKCommand1095)(  );
extern int(*AGKCommand1096)(  );
extern int(*AGKCommand1097)(  );
extern void(*AGKCommand1098)(  );
extern void(*AGKCommand1099)(  );
extern void(*AGKCommand1100)( int, int, int, int );
extern void(*AGKCommand1101)( int, int, int, int, float, float );
extern void(*AGKCommand1102)( float, float, float );
extern void(*AGKCommand1103)( int, int, float );
extern void(*AGKCommand1104)( int, int, float, float, float );
extern void(*AGKCommand1105)( int );
extern void(*AGKCommand1106)(  );
extern void(*AGKCommand1107)(  );
extern void(*AGKCommand1108)( unsigned int, const char* );
extern unsigned int(*AGKCommand1109)( const char* );
extern void(*AGKCommand1110)( unsigned int, const char*, const char* );
extern void(*AGKCommand1111)( unsigned int );
extern void(*AGKCommand1112)( const char*, const char* );
extern void(*AGKCommand1113)( const char * );
extern void(*AGKCommand1114)( const char* );
extern void(*AGKCommand1115)( const char*, const char* );
extern void(*AGKCommand1116)( const char*, const char*, const char* );
extern void(*AGKCommand1117)(  );
extern void(*AGKCommand1118)( const char*, const char* );
extern void(*AGKCommand1119)( const char* );
extern void(*AGKCommand1120)( const char*, int );
extern void(*AGKCommand1121)(  );
extern int(*AGKCommand1122)( int );
extern void(*AGKCommand1123)( int );
extern char*(*AGKCommand1124)( int );
extern char*(*AGKCommand1125)( int );
extern int(*AGKCommand1126)(  );
extern void(*AGKCommand1127)(  );
extern char*(*AGKCommand1128)( int );
extern void(*AGKCommand1129)( const char*, const char* );
extern void(*AGKCommand1130)( const char* );
extern void(*AGKCommand1131)( const char* );
extern int(*AGKCommand1132)(  );
extern char*(*AGKCommand1133)(  );
extern char*(*AGKCommand1134)(  );
extern char*(*AGKCommand1135)(  );
extern void(*AGKCommand1136)(  );
extern void(*AGKCommand1137)(  );
extern void(*AGKCommand1138)( const char*, const char*, const char*, const char*, const char* );
extern void(*AGKCommand1139)( const char*, const char*, const char*, const char*, const char*, const char* );
extern void(*AGKCommand1140)( const char*, const char* );
extern void(*AGKCommand1141)( const char*, int, int, int, int );
extern void(*AGKCommand1142)(  );
extern void(*AGKCommand1143)(  );
extern int(*AGKCommand1144)(  );
extern int(*AGKCommand1145)(  );
extern char*(*AGKCommand1146)( int );
extern char*(*AGKCommand1147)( int );
extern void(*AGKCommand1148)( int );
extern int(*AGKCommand1149)(  );
extern char*(*AGKCommand1150)(  );
extern void(*AGKCommand1151)( const char*, const char*, const char* );
extern int(*AGKCommand1152)(  );
extern char*(*AGKCommand1153)(  );
extern void(*AGKCommand1154)(  );
extern void(*AGKCommand1155)( int, int, const char * );
extern void(*AGKCommand1156)( int );
extern int(*AGKCommand1157)( int );
extern int(*AGKCommand1158)( int );
extern char*(*AGKCommand1159)( int );
extern unsigned int(*AGKCommand1160)( unsigned int, unsigned int, unsigned int );
extern unsigned int(*AGKCommand1161)( unsigned int );
extern unsigned int(*AGKCommand1162)( unsigned int );
extern unsigned int(*AGKCommand1163)( unsigned int );
extern void(*AGKCommand1164)( float, float, float, float, unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1165)( float, float, float, float, unsigned int, unsigned int );
extern void(*AGKCommand1166)( float, float, float, float, unsigned int, unsigned int, unsigned int, unsigned int, int );
extern void(*AGKCommand1167)( float, float, float, float, unsigned int, unsigned int, int );
extern unsigned int(*AGKCommand1168)( unsigned int );
extern void(*AGKCommand1169)( unsigned int, unsigned int );
extern int(*AGKCommand1170)( unsigned int );
extern void(*AGKCommand1171)( unsigned int );
extern void(*AGKCommand1172)( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int );
extern int(*AGKCommand1173)( unsigned int );
extern int(*AGKCommand1174)( unsigned int, unsigned int );
extern int(*AGKCommand1175)( unsigned int, unsigned int );
extern int(*AGKCommand1176)( unsigned int, unsigned int );
extern int(*AGKCommand1177)( unsigned int, unsigned int );
extern float(*AGKCommand1178)( unsigned int, unsigned int );
extern char*(*AGKCommand1179)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1180)( unsigned int, unsigned int, int );
extern void(*AGKCommand1181)( unsigned int, unsigned int, int );
extern void(*AGKCommand1182)( unsigned int, unsigned int, int );
extern void(*AGKCommand1183)( unsigned int, unsigned int, int );
extern void(*AGKCommand1184)( unsigned int, unsigned int, float );
extern void(*AGKCommand1185)( unsigned int, unsigned int, const char* );
extern void(*AGKCommand1186)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand1187)( unsigned int );
extern void(*AGKCommand1188)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand1189)( unsigned int );
extern void(*AGKCommand1190)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand1191)( unsigned int );
extern void(*AGKCommand1192)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand1193)( unsigned int );
extern void(*AGKCommand1194)( unsigned int, const char* );
extern unsigned int(*AGKCommand1195)( const char* );
extern void(*AGKCommand1196)( const char*, unsigned int );
extern void(*AGKCommand1197)( unsigned int, unsigned int, unsigned int );
extern unsigned int(*AGKCommand1198)( unsigned int, unsigned int );
extern void(*AGKCommand1199)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand1200)( unsigned int );
extern void(*AGKCommand1201)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1202)( unsigned int, unsigned int );
extern void(*AGKCommand1203)( unsigned int, unsigned int, float, float, float );
extern void(*AGKCommand1204)( unsigned int, unsigned int, float, float, float );
extern void(*AGKCommand1205)( unsigned int, unsigned int, float, float );
extern float(*AGKCommand1206)( unsigned int, unsigned int );
extern float(*AGKCommand1207)( unsigned int, unsigned int );
extern float(*AGKCommand1208)( unsigned int, unsigned int );
extern float(*AGKCommand1209)( unsigned int, unsigned int );
extern float(*AGKCommand1210)( unsigned int, unsigned int );
extern float(*AGKCommand1211)( unsigned int, unsigned int );
extern float(*AGKCommand1212)( unsigned int, unsigned int );
extern float(*AGKCommand1213)( unsigned int, unsigned int );
extern void(*AGKCommand1214)( int );
extern void(*AGKCommand1215)( unsigned int, float, float, float );
extern unsigned int(*AGKCommand1216)( float, float, float );
extern void(*AGKCommand1217)( unsigned int, float, float, int );
extern unsigned int(*AGKCommand1218)( float, float, int );
extern void(*AGKCommand1219)( unsigned int, float, int, int );
extern unsigned int(*AGKCommand1220)( float, int, int );
extern void(*AGKCommand1221)( unsigned int, float, float, int );
extern unsigned int(*AGKCommand1222)( float, float, int );
extern void(*AGKCommand1223)( unsigned int, float, float, int );
extern unsigned int(*AGKCommand1224)( float, float, int );
extern void(*AGKCommand1225)( unsigned int, float, float );
extern unsigned int(*AGKCommand1226)( float, float );
extern void(*AGKCommand1227)( unsigned int );
extern unsigned int(*AGKCommand1228)(  );
extern unsigned int(*AGKCommand1229)( const char*, float, float, float, int, int );
extern void(*AGKCommand1230)( unsigned int, const char*, float, float, float, int, int );
extern unsigned int(*AGKCommand1231)( unsigned int, unsigned int );
extern void(*AGKCommand1232)( unsigned int, unsigned int, unsigned int );
extern unsigned int(*AGKCommand1233)( const char* );
extern unsigned int(*AGKCommand1234)( const char*, float );
extern void(*AGKCommand1235)( unsigned int, const char* );
extern void(*AGKCommand1236)( unsigned int, const char*, float );
extern unsigned int(*AGKCommand1237)( const char* );
extern void(*AGKCommand1238)( unsigned int, const char* );
extern void(*AGKCommand1239)( unsigned int, const char * );
extern unsigned int(*AGKCommand1240)( unsigned int );
extern void(*AGKCommand1241)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand1242)( unsigned int );
extern void(*AGKCommand1243)( unsigned int, unsigned int );
extern int(*AGKCommand1244)( unsigned int );
extern void(*AGKCommand1245)( unsigned int );
extern void(*AGKCommand1246)( unsigned int );
extern void(*AGKCommand1247)(  );
extern unsigned int(*AGKCommand1248)( unsigned int );
extern unsigned int(*AGKCommand1249)( unsigned int, int );
extern unsigned int(*AGKCommand1250)( unsigned int );
extern unsigned int(*AGKCommand1251)( unsigned int, const char * );
extern unsigned int(*AGKCommand1252)( unsigned int );
extern char*(*AGKCommand1253)( unsigned int, unsigned int );
extern void(*AGKCommand1254)( unsigned int, unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1255)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1256)( unsigned int, unsigned int, unsigned int );
extern char*(*AGKCommand1257)( unsigned int, unsigned int );
extern char*(*AGKCommand1258)( unsigned int, unsigned int );
extern void(*AGKCommand1259)( unsigned int, unsigned int, unsigned int, float, float );
extern void(*AGKCommand1260)( unsigned int, unsigned int, unsigned int, float, float );
extern float(*AGKCommand1261)( unsigned int, unsigned int );
extern float(*AGKCommand1262)( unsigned int, unsigned int );
extern float(*AGKCommand1263)( unsigned int, unsigned int );
extern float(*AGKCommand1264)( unsigned int, unsigned int );
extern float(*AGKCommand1265)( unsigned int, unsigned int );
extern float(*AGKCommand1266)( unsigned int, unsigned int );
extern void(*AGKCommand1267)( unsigned int, unsigned int );
extern void(*AGKCommand1268)( unsigned int, unsigned int, unsigned int );
extern int(*AGKCommand1269)( unsigned int );
extern char*(*AGKCommand1270)( unsigned int, int );
extern void(*AGKCommand1271)( unsigned int, const char *, float, float, int, float );
extern void(*AGKCommand1272)( unsigned int, const char *, float, float );
extern void(*AGKCommand1273)( unsigned int );
extern void(*AGKCommand1274)( unsigned int );
extern void(*AGKCommand1275)( unsigned int, float );
extern int(*AGKCommand1276)( unsigned int );
extern int(*AGKCommand1277)( unsigned int );
extern float(*AGKCommand1278)( unsigned int );
extern float(*AGKCommand1279)( unsigned int, const char * );
extern void(*AGKCommand1280)( unsigned int, unsigned int, float, float, float );
extern void(*AGKCommand1281)( unsigned int, unsigned int, float, float, float );
extern void(*AGKCommand1282)( unsigned int, unsigned int, float, float, float, float );
extern void(*AGKCommand1283)( unsigned int, unsigned int, float, float, float, float );
extern void(*AGKCommand1284)( unsigned int, unsigned int, int );
extern void(*AGKCommand1285)( unsigned int, unsigned int, float );
extern void(*AGKCommand1286)( unsigned int, unsigned int, float );
extern void(*AGKCommand1287)( unsigned int, unsigned int, float );
extern char*(*AGKCommand1288)( unsigned int, unsigned int );
extern float(*AGKCommand1289)( unsigned int, unsigned int );
extern float(*AGKCommand1290)( unsigned int, unsigned int );
extern float(*AGKCommand1291)( unsigned int, unsigned int );
extern float(*AGKCommand1292)( unsigned int, unsigned int );
extern float(*AGKCommand1293)( unsigned int, unsigned int );
extern float(*AGKCommand1294)( unsigned int, unsigned int );
extern float(*AGKCommand1295)( unsigned int, unsigned int );
extern float(*AGKCommand1296)( unsigned int, unsigned int );
extern float(*AGKCommand1297)( unsigned int, unsigned int );
extern float(*AGKCommand1298)( unsigned int, unsigned int );
extern float(*AGKCommand1299)( unsigned int, unsigned int );
extern float(*AGKCommand1300)( unsigned int, unsigned int );
extern float(*AGKCommand1301)( unsigned int, unsigned int );
extern float(*AGKCommand1302)( unsigned int, unsigned int );
extern float(*AGKCommand1303)( unsigned int, unsigned int );
extern float(*AGKCommand1304)( unsigned int, unsigned int );
extern float(*AGKCommand1305)( unsigned int, unsigned int );
extern float(*AGKCommand1306)( unsigned int, unsigned int );
extern float(*AGKCommand1307)( unsigned int, unsigned int );
extern float(*AGKCommand1308)( unsigned int, unsigned int );
extern void(*AGKCommand1309)( unsigned int, float, float, float );
extern void(*AGKCommand1310)( unsigned int, float, float, float );
extern void(*AGKCommand1311)( unsigned int, float, float, float, float );
extern void(*AGKCommand1312)( unsigned int, float, float, float );
extern void(*AGKCommand1313)( unsigned int, float, float, float );
extern void(*AGKCommand1314)( unsigned int, float );
extern void(*AGKCommand1315)( unsigned int, float );
extern void(*AGKCommand1316)( unsigned int, float );
extern void(*AGKCommand1317)( unsigned int, float );
extern void(*AGKCommand1318)( unsigned int, float );
extern void(*AGKCommand1319)( unsigned int, float );
extern void(*AGKCommand1320)( unsigned int, float );
extern void(*AGKCommand1321)( unsigned int, float );
extern void(*AGKCommand1322)( unsigned int, float );
extern float(*AGKCommand1323)( unsigned int );
extern float(*AGKCommand1324)( unsigned int );
extern float(*AGKCommand1325)( unsigned int );
extern float(*AGKCommand1326)( unsigned int );
extern float(*AGKCommand1327)( unsigned int );
extern float(*AGKCommand1328)( unsigned int );
extern float(*AGKCommand1329)( unsigned int );
extern float(*AGKCommand1330)( unsigned int );
extern float(*AGKCommand1331)( unsigned int );
extern float(*AGKCommand1332)( unsigned int );
extern float(*AGKCommand1333)( unsigned int );
extern float(*AGKCommand1334)( unsigned int );
extern float(*AGKCommand1335)( unsigned int );
extern float(*AGKCommand1336)( unsigned int );
extern float(*AGKCommand1337)( unsigned int );
extern float(*AGKCommand1338)( unsigned int );
extern float(*AGKCommand1339)( unsigned int );
extern float(*AGKCommand1340)( unsigned int );
extern float(*AGKCommand1341)( unsigned int );
extern float(*AGKCommand1342)( unsigned int );
extern float(*AGKCommand1343)( unsigned int );
extern float(*AGKCommand1344)( unsigned int );
extern float(*AGKCommand1345)( unsigned int );
extern float(*AGKCommand1346)( unsigned int );
extern float(*AGKCommand1347)( unsigned int );
extern float(*AGKCommand1348)( unsigned int );
extern void(*AGKCommand1349)( unsigned int, float, float, float, float );
extern void(*AGKCommand1350)( unsigned int );
extern float(*AGKCommand1351)( unsigned int, float, float );
extern void(*AGKCommand1352)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1353)( unsigned int, unsigned int );
extern void(*AGKCommand1354)( unsigned int, unsigned int );
extern void(*AGKCommand1355)( unsigned int, int, int, int, int );
extern void(*AGKCommand1356)( unsigned int, int, int, int );
extern void(*AGKCommand1357)( unsigned int, int );
extern void(*AGKCommand1358)( unsigned int, int );
extern void(*AGKCommand1359)( unsigned int, unsigned int, float, float );
extern void(*AGKCommand1360)( unsigned int, unsigned int, float, float );
extern void(*AGKCommand1361)( unsigned int, int );
extern void(*AGKCommand1362)( unsigned int, int );
extern void(*AGKCommand1363)( unsigned int, int );
extern void(*AGKCommand1364)( unsigned int, float );
extern void(*AGKCommand1365)( unsigned int, float, float );
extern void(*AGKCommand1366)( unsigned int, int );
extern void(*AGKCommand1367)( unsigned int, int );
extern void(*AGKCommand1368)( unsigned int, int );
extern void(*AGKCommand1369)( unsigned int, int );
extern int(*AGKCommand1370)( unsigned int );
extern int(*AGKCommand1371)( unsigned int );
extern float(*AGKCommand1372)( unsigned int );
extern int(*AGKCommand1373)( unsigned int );
extern int(*AGKCommand1374)( unsigned int );
extern int(*AGKCommand1375)( unsigned int );
extern int(*AGKCommand1376)( unsigned int );
extern char*(*AGKCommand1377)( unsigned int );
extern void(*AGKCommand1378)( unsigned int, const char *, float, float, float, float );
extern void(*AGKCommand1379)( unsigned int, const char *, unsigned int, float, float, float, float );
extern void(*AGKCommand1380)( unsigned int, const char * );
extern void(*AGKCommand1381)( unsigned int );
extern float(*AGKCommand1382)( float, float, float );
extern float(*AGKCommand1383)( float, float, float );
extern float(*AGKCommand1384)( float, float );
extern float(*AGKCommand1385)( float, float );
extern float(*AGKCommand1386)( float, float );
extern void(*AGKCommand1387)( unsigned int, int );
extern int(*AGKCommand1388)( unsigned int, float, float, float, float, float, float );
extern int(*AGKCommand1389)( unsigned int, float, float, float, float, float, float, float );
extern int(*AGKCommand1390)( unsigned int, float, float, float, float, float, float, float );
extern unsigned int(*AGKCommand1391)(  );
extern unsigned int(*AGKCommand1392)( unsigned int );
extern float(*AGKCommand1393)( unsigned int );
extern float(*AGKCommand1394)( unsigned int );
extern float(*AGKCommand1395)( unsigned int );
extern float(*AGKCommand1396)( unsigned int );
extern float(*AGKCommand1397)( unsigned int );
extern float(*AGKCommand1398)( unsigned int );
extern float(*AGKCommand1399)( unsigned int );
extern float(*AGKCommand1400)( unsigned int );
extern float(*AGKCommand1401)( unsigned int );
extern float(*AGKCommand1402)( unsigned int );
extern float(*AGKCommand1403)( unsigned int );
extern float(*AGKCommand1404)( unsigned int );
extern float(*AGKCommand1405)( unsigned int );
extern void(*AGKCommand1406)( int );
extern void(*AGKCommand1407)( int, int, int );
extern void(*AGKCommand1408)( int, int, int );
extern void(*AGKCommand1409)( float, float );
extern int(*AGKCommand1410)(  );
extern void(*AGKCommand1411)( int );
extern void(*AGKCommand1412)( int, int, int );
extern void(*AGKCommand1413)( int, int, int );
extern void(*AGKCommand1414)( int, int, int );
extern void(*AGKCommand1415)( float, float );
extern void(*AGKCommand1416)( int );
extern void(*AGKCommand1417)( float, float );
extern unsigned int(*AGKCommand1418)( const char*, const char* );
extern void(*AGKCommand1419)( unsigned int, const char*, const char* );
extern unsigned int(*AGKCommand1420)( const char* );
extern void(*AGKCommand1421)( unsigned int, const char* );
extern unsigned int(*AGKCommand1422)( const char* );
extern void(*AGKCommand1423)( unsigned int, const char* );
extern void(*AGKCommand1424)( unsigned int, const char *, float, float, float, float );
extern void(*AGKCommand1425)( unsigned int, const char *, unsigned int, float, float, float, float );
extern void(*AGKCommand1426)( unsigned int, float, float, float );
extern void(*AGKCommand1427)( unsigned int, float, float, float, float );
extern void(*AGKCommand1428)( unsigned int, float, float, float );
extern void(*AGKCommand1429)( unsigned int, float );
extern void(*AGKCommand1430)( unsigned int, float );
extern void(*AGKCommand1431)( unsigned int, float );
extern void(*AGKCommand1432)( unsigned int, float );
extern void(*AGKCommand1433)( unsigned int, float );
extern void(*AGKCommand1434)( unsigned int, float );
extern void(*AGKCommand1435)( unsigned int, float );
extern void(*AGKCommand1436)( unsigned int, float );
extern void(*AGKCommand1437)( unsigned int, float );
extern float(*AGKCommand1438)( unsigned int );
extern float(*AGKCommand1439)( unsigned int );
extern float(*AGKCommand1440)( unsigned int );
extern float(*AGKCommand1441)( unsigned int );
extern float(*AGKCommand1442)( unsigned int );
extern float(*AGKCommand1443)( unsigned int );
extern float(*AGKCommand1444)( unsigned int );
extern float(*AGKCommand1445)( unsigned int );
extern float(*AGKCommand1446)( unsigned int );
extern float(*AGKCommand1447)( unsigned int );
extern void(*AGKCommand1448)( unsigned int, float, float, float, float );
extern void(*AGKCommand1449)( unsigned int, float, float );
extern void(*AGKCommand1450)( unsigned int, float );
extern void(*AGKCommand1451)( unsigned int, float );
extern void(*AGKCommand1452)( unsigned int, float );
extern void(*AGKCommand1453)( unsigned int, float, float, float, float );
extern void(*AGKCommand1454)( unsigned int, int );
extern float(*AGKCommand1455)( unsigned int );
extern void(*AGKCommand1456)( unsigned int, float, float, float, float, int, int, int );
extern int(*AGKCommand1457)( unsigned int );
extern void(*AGKCommand1458)( unsigned int );
extern void(*AGKCommand1459)(  );
extern void(*AGKCommand1460)( unsigned int, float, float, float );
extern void(*AGKCommand1461)( unsigned int, int, int, int );
extern void(*AGKCommand1462)( unsigned int, float );
extern void(*AGKCommand1463)( unsigned int, int );
extern void(*AGKCommand1464)( float, float, float );
extern void(*AGKCommand1465)( int, int, int );
extern void(*AGKCommand1466)( int );
extern void(*AGKCommand1467)( int, int, int );
extern void(*AGKCommand1468)(  );
extern void(*AGKCommand1469)( float );
extern void(*AGKCommand1470)( float, float, float );
extern void(*AGKCommand1471)( unsigned int );
extern void(*AGKCommand1472)(  );
extern void(*AGKCommand1473)(  );
extern void(*AGKCommand1474)(  );
extern void(*AGKCommand1475)(  );
extern int(*AGKCommand1476)(  );
extern int(*AGKCommand1477)(  );
extern int(*AGKCommand1478)(  );
extern void(*AGKCommand1479)( unsigned int );
extern void(*AGKCommand1480)( unsigned int, float, float, float );
extern void(*AGKCommand1481)( unsigned int, unsigned int );
extern void(*AGKCommand1482)( unsigned int );
extern void(*AGKCommand1483)( unsigned int, float );
extern void(*AGKCommand1484)( unsigned int, int );
extern void(*AGKCommand1485)( unsigned int, int, float, float );
extern void(*AGKCommand1486)( unsigned int, int );
extern void(*AGKCommand1487)( unsigned int, int, float, float );
extern void(*AGKCommand1488)( unsigned int, int );
extern void(*AGKCommand1489)( unsigned int, int, float, float, float );
extern void(*AGKCommand1490)( unsigned int, int, unsigned int );
extern void(*AGKCommand1491)( unsigned int );
extern void(*AGKCommand1492)( unsigned int );
extern void(*AGKCommand1493)( unsigned int );
extern void(*AGKCommand1494)( unsigned int );
extern void(*AGKCommand1495)( unsigned int );
extern void(*AGKCommand1496)( unsigned int );
extern void(*AGKCommand1497)( unsigned int );
extern int(*AGKCommand1498)( float, float, float, float );
extern void(*AGKCommand1499)( unsigned int, float, float, float );
extern void(*AGKCommand1500)( unsigned int, float, float, float );
extern void(*AGKCommand1501)( unsigned int );
extern void(*AGKCommand1502)( unsigned int, int, int );
extern int(*AGKCommand1503)( unsigned int );
extern int(*AGKCommand1504)( unsigned int );
extern void(*AGKCommand1505)( unsigned int, float );
extern float(*AGKCommand1506)( unsigned int );
extern void(*AGKCommand1507)( unsigned int, int );
extern void(*AGKCommand1508)( unsigned int, float );
extern float(*AGKCommand1509)( unsigned int );
extern void(*AGKCommand1510)( unsigned int, float );
extern float(*AGKCommand1511)( unsigned int );
extern void(*AGKCommand1512)( unsigned int, int );
extern void(*AGKCommand1513)( unsigned int, float );
extern void(*AGKCommand1514)( unsigned int, float, float, float, float );
extern void(*AGKCommand1515)( unsigned int, unsigned int, float );
extern float(*AGKCommand1516)( unsigned int );
extern float(*AGKCommand1517)( unsigned int );
extern float(*AGKCommand1518)( unsigned int );
extern void(*AGKCommand1519)( unsigned int, float, float, float, float );
extern void(*AGKCommand1520)( unsigned int, unsigned int, float );
extern float(*AGKCommand1521)( unsigned int );
extern float(*AGKCommand1522)( unsigned int );
extern float(*AGKCommand1523)( unsigned int );
extern void(*AGKCommand1524)( unsigned int, float, float );
extern float(*AGKCommand1525)( unsigned int );
extern float(*AGKCommand1526)( unsigned int );
extern void(*AGKCommand1527)( unsigned int, float, float );
extern float(*AGKCommand1528)( unsigned int );
extern float(*AGKCommand1529)( unsigned int );
extern void(*AGKCommand1530)( unsigned int, float );
extern void(*AGKCommand1531)( unsigned int, float );
extern float(*AGKCommand1532)( unsigned int );
extern int(*AGKCommand1533)( unsigned int );
extern float(*AGKCommand1534)(  );
extern float(*AGKCommand1535)(  );
extern float(*AGKCommand1536)(  );
extern int(*AGKCommand1537)( int );
extern int(*AGKCommand1538)(  );
extern int(*AGKCommand1539)(  );
extern int(*AGKCommand1540)( unsigned int, unsigned int, int );
extern int(*AGKCommand1541)( unsigned int, int );
extern void(*AGKCommand1542)( unsigned int, int );
extern void(*AGKCommand1543)( unsigned int );
extern int(*AGKCommand1544)( unsigned int, unsigned int, int, int, int );
extern int(*AGKCommand1545)( unsigned int, unsigned int, int, int, int );
extern int(*AGKCommand1546)( unsigned int, unsigned int, int, int );
extern int(*AGKCommand1547)( unsigned int, unsigned int, int );
extern int(*AGKCommand1548)( unsigned int, unsigned int, int, float );
extern void(*AGKCommand1549)( unsigned int, float, float );
extern void(*AGKCommand1550)( unsigned int, float, float );
extern void(*AGKCommand1551)( unsigned int, float, float, float );
extern void(*AGKCommand1552)( unsigned int, float, float );
extern void(*AGKCommand1553)( unsigned int, float );
extern void(*AGKCommand1554)( unsigned int, int );
extern int(*AGKCommand1555)( unsigned int );
extern int(*AGKCommand1556)( unsigned int );
extern int(*AGKCommand1557)( unsigned int );
extern void(*AGKCommand1558)( unsigned int );
extern void(*AGKCommand1559)( unsigned int, int );
extern void(*AGKCommand1560)( unsigned int, float );
extern void(*AGKCommand1561)( unsigned int, float );
extern void(*AGKCommand1562)( unsigned int, int );
extern void(*AGKCommand1563)( unsigned int, float );
extern void(*AGKCommand1564)( unsigned int, int );
extern void(*AGKCommand1565)( unsigned int, int );
extern void(*AGKCommand1566)( unsigned int, float );
extern void(*AGKCommand1567)( unsigned int, float );
extern void(*AGKCommand1568)( int, int, int, int );
extern void(*AGKCommand1569)( int, int, float );
extern void(*AGKCommand1570)( int, int, int, int, int );
extern void(*AGKCommand1571)( int, int, int, int, int );
extern void(*AGKCommand1572)( int, int, int, int, int );
extern int(*AGKCommand1573)( unsigned int, const char* );
extern int(*AGKCommand1574)( unsigned int, const char* );
extern int(*AGKCommand1575)(  );
extern int(*AGKCommand1576)( float, float, float );
extern void(*AGKCommand1577)( unsigned int, float, float, float );
extern void(*AGKCommand1578)( unsigned int );
extern float(*AGKCommand1579)( unsigned int );
extern float(*AGKCommand1580)( unsigned int );
extern float(*AGKCommand1581)( unsigned int );
extern float(*AGKCommand1582)( unsigned int, unsigned int );
extern float(*AGKCommand1583)( unsigned int );
extern float(*AGKCommand1584)( unsigned int, unsigned int );
extern void(*AGKCommand1585)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1586)( unsigned int, float );
extern void(*AGKCommand1587)( unsigned int, unsigned int );
extern int(*AGKCommand1588)(  );
extern void(*AGKCommand1589)( unsigned int );
extern int(*AGKCommand1590)( unsigned int );
extern void(*AGKCommand1591)( unsigned int, int, int, int );
extern int(*AGKCommand1592)( unsigned int, unsigned int, int, int, int );
extern int(*AGKCommand1593)( unsigned int, float );
extern int(*AGKCommand1594)( unsigned int );
extern float(*AGKCommand1595)( unsigned int );
extern int(*AGKCommand1596)( unsigned int, float, int );
extern int(*AGKCommand1597)( unsigned int, int );
extern void(*AGKCommand1598)( unsigned int, int );
extern int(*AGKCommand1599)( unsigned int );
extern void(*AGKCommand1600)( unsigned int, int, int, float );
extern int(*AGKCommand1601)( unsigned int, unsigned int, int, int, float );
extern void(*AGKCommand1602)( unsigned int, float );
extern int(*AGKCommand1603)( unsigned int, unsigned int, float, int, int );
extern void(*AGKCommand1604)( unsigned int, unsigned int );
extern void(*AGKCommand1605)( unsigned int, unsigned int, unsigned int, int, float, float );
extern void(*AGKCommand1606)( unsigned int, unsigned int, unsigned int, int, int );
extern void(*AGKCommand1607)(  );
extern void(*AGKCommand1608)( unsigned int );
extern int(*AGKCommand1609)( unsigned int );
extern int(*AGKCommand1610)( unsigned int );
extern int(*AGKCommand1611)( unsigned int );
extern void(*AGKCommand1612)( unsigned int, int );
extern void(*AGKCommand1613)( unsigned int, int );
extern void(*AGKCommand1614)( float, float );
extern void(*AGKCommand1615)( float, float );
extern void(*AGKCommand1616)( float );
extern void(*AGKCommand1617)( unsigned int, int );
extern void(*AGKCommand1618)( unsigned int, int, int, int, float );
extern void(*AGKCommand1619)( unsigned int );
extern int(*AGKCommand1620)( unsigned int );
extern void(*AGKCommand1621)( unsigned int, float );
extern void(*AGKCommand1622)( unsigned int, float );
extern void(*AGKCommand1623)( unsigned int, float );
extern void(*AGKCommand1624)( unsigned int, float );
extern void(*AGKCommand1625)( unsigned int, float );
extern float(*AGKCommand1626)( unsigned int );
extern float(*AGKCommand1627)( unsigned int );
extern void(*AGKCommand1628)( unsigned int, int, float );
extern void(*AGKCommand1629)( unsigned int, float );
extern void(*AGKCommand1630)( unsigned int );
extern void(*AGKCommand1631)( unsigned int );
extern void(*AGKCommand1632)( unsigned int );
extern void(*AGKCommand1633)( unsigned int, int );
extern void(*AGKCommand1634)( unsigned int, float, float, float );
extern void(*AGKCommand1635)( int, int );
extern void(*AGKCommand1636)( int, int, int );
extern void(*AGKCommand1637)( int, int, int, int );
extern void(*AGKCommand1638)( int );
extern void(*AGKCommand1639)(  );
extern void(*AGKCommand1640)(  );
extern void(*AGKCommand1641)(  );
extern void(*AGKCommand1642)( int );
extern void(*AGKCommand1643)( int, int );
extern char*(*AGKCommand1644)(  );
extern char*(*AGKCommand1645)(  );
extern char*(*AGKCommand1646)(  );
extern char*(*AGKCommand1647)(  );
extern char*(*AGKCommand1648)(  );
extern void(*AGKCommand1649)( int );
extern void(*AGKCommand1650)( const char * );
extern void(*AGKCommand1651)( int );
extern int(*AGKCommand1652)(  );
extern void(*AGKCommand1653)(  );
extern float(*AGKCommand1654)(  );
extern void(*AGKCommand1655)( const char * );
extern void(*AGKCommand1656)( unsigned int );
extern void(*AGKCommand1657)( int );
extern int(*AGKCommand1658)(  );
extern int(*AGKCommand1659)(  );
extern int(*AGKCommand1660)(  );
extern char*(*AGKCommand1661)(  );
extern int(*AGKCommand1662)(  );
extern float(*AGKCommand1663)( float );
extern float(*AGKCommand1664)( float );
extern float(*AGKCommand1665)( float );
extern float(*AGKCommand1666)( float );
extern float(*AGKCommand1667)( float );
extern float(*AGKCommand1668)( float );
extern float(*AGKCommand1669)( float );
extern float(*AGKCommand1670)( float );
extern float(*AGKCommand1671)( float );
extern float(*AGKCommand1672)( float, float );
extern float(*AGKCommand1673)( float );
extern float(*AGKCommand1674)( float );
extern float(*AGKCommand1675)( float );
extern float(*AGKCommand1676)( float, float );
extern int(*AGKCommand1677)( float );
extern int(*AGKCommand1678)( float );
extern int(*AGKCommand1679)( float );
extern int(*AGKCommand1680)( float );
extern float(*AGKCommand1681)( float );
extern float(*AGKCommand1682)( float );
extern int(*AGKCommand1683)( int, int );
extern float(*AGKCommand1684)( float, float );
extern float(*AGKCommand1685)( float, float );
extern float(*AGKCommand1686)( float );
extern int(*AGKCommand1687)(  );
extern int(*AGKCommand1688)( unsigned int, unsigned int );
extern int(*AGKCommand1689)( unsigned int );
extern void(*AGKCommand1690)( float );
extern int(*AGKCommand1691)( const char * );
extern void(*AGKCommand1692)(  );
extern void(*AGKCommand1693)( float, float, float, float );
extern void(*AGKCommand1694)( unsigned int );
extern void(*AGKCommand1695)(  );
extern void(*AGKCommand1696)(  );
extern void(*AGKCommand1697)(  );
extern int(*AGKCommand1698)(  );
extern float(*AGKCommand1699)(  );
extern float(*AGKCommand1700)(  );
extern void(*AGKCommand1701)( float );
extern float(*AGKCommand1702)(  );
extern float(*AGKCommand1703)(  );
extern void(*AGKCommand1704)( float );
extern void(*AGKCommand1705)( const char* );
extern char*(*AGKCommand1706)(  );
extern char*(*AGKCommand1707)(  );
extern char*(*AGKCommand1708)(  );
extern int(*AGKCommand1709)( const char* );
extern int(*AGKCommand1710)( const char* );
extern void(*AGKCommand1711)( const char* );
extern int(*AGKCommand1712)(  );
extern int(*AGKCommand1713)(  );
extern int(*AGKCommand1714)(  );
extern int(*AGKCommand1715)(  );
extern int(*AGKCommand1716)(  );
extern void(*AGKCommand1717)( int );
extern void(*AGKCommand1718)( float, float );
extern int(*AGKCommand1719)(  );
extern int(*AGKCommand1720)(  );
extern char*(*AGKCommand1721)(  );
extern char*(*AGKCommand1722)(  );
extern void(*AGKCommand1723)( const char * );
extern unsigned int(*AGKCommand1724)( const char *, const char * );
extern unsigned int(*AGKCommand1725)( unsigned int );
extern void(*AGKCommand1726)( unsigned int );
extern void(*AGKCommand1727)( const char* );
extern void(*AGKCommand1728)( const char* );
extern void(*AGKCommand1729)( const char* );
extern void(*AGKCommand1730)( const char*, const char* );
extern void(*AGKCommand1731)(  );
extern int(*AGKCommand1732)(  );
extern void(*AGKCommand1733)( const char*, const char* );
extern int(*AGKCommand1734)(  );
extern char*(*AGKCommand1735)(  );
extern int(*AGKCommand1736)(  );
extern void(*AGKCommand1737)( int );
extern void(*AGKCommand1738)( const char* );
extern int(*AGKCommand1739)(  );
extern void(*AGKCommand1740)(  );
extern void(*AGKCommand1741)(  );
extern float(*AGKCommand1742)(  );
extern float(*AGKCommand1743)(  );
extern float(*AGKCommand1744)(  );
extern int(*AGKCommand1745)(  );
extern void(*AGKCommand1746)(  );
extern void(*AGKCommand1747)(  );
extern int(*AGKCommand1748)(  );
extern char*(*AGKCommand1749)(  );
extern char*(*AGKCommand1750)(  );
extern void(*AGKCommand1751)( int, const char* );
extern void(*AGKCommand1752)( const char* );
extern void(*AGKCommand1753)( const char*, int );
extern void(*AGKCommand1754)(  );
extern void(*AGKCommand1755)(  );
extern void(*AGKCommand1756)( const char* );
extern void(*AGKCommand1757)( const char *, const char * );
extern char*(*AGKCommand1758)( const char *, const char * );
extern void(*AGKCommand1759)( const char * );
extern void(*AGKCommand1760)(  );
extern void(*AGKCommand1761)( const char * );
extern unsigned int(*AGKCommand1762)(  );
extern void(*AGKCommand1763)( int );
extern void(*AGKCommand1764)(  );
extern void(*AGKCommand1765)( unsigned int );
extern void(*AGKCommand1766)( unsigned int, unsigned int );
extern void(*AGKCommand1767)( int, int );
extern void(*AGKCommand1768)( int, int );
extern int(*AGKCommand1769)( int );
extern int(*AGKCommand1770)( int );
extern int(*AGKCommand1771)(  );
extern void(*AGKCommand1772)( int );
extern int(*AGKCommand1773)(  );
extern void(*AGKCommand1774)( int, int );
extern void(*AGKCommand1775)( float );
extern void(*AGKCommand1776)( float );
extern void(*AGKCommand1777)( float );
extern void(*AGKCommand1778)( int );
extern unsigned int(*AGKCommand1779)(  );
extern unsigned int(*AGKCommand1780)( const char*, const char* );
extern void(*AGKCommand1781)( unsigned int, const char*, const char* );
extern void(*AGKCommand1782)( unsigned int, float, float, float );
extern unsigned int(*AGKCommand1783)( float, float, float );
extern int(*AGKCommand1784)( unsigned int );
extern void(*AGKCommand1785)( unsigned int );
extern void(*AGKCommand1786)( unsigned int, float, float, float );
extern void(*AGKCommand1787)( unsigned int, float );
extern void(*AGKCommand1788)( unsigned int, float, float, float, float, float, float );
extern void(*AGKCommand1789)( unsigned int, float, float, float, float );
extern void(*AGKCommand1790)( unsigned int, float, float );
extern void(*AGKCommand1791)( unsigned int, float, float );
extern void(*AGKCommand1792)( unsigned int, float );
extern void(*AGKCommand1793)( unsigned int, float );
extern void(*AGKCommand1794)( unsigned int, int );
extern void(*AGKCommand1795)( unsigned int );
extern void(*AGKCommand1796)( unsigned int, unsigned int );
extern void(*AGKCommand1797)( unsigned int, int );
extern void(*AGKCommand1798)( unsigned int, int );
extern void(*AGKCommand1799)( unsigned int, int );
extern void(*AGKCommand1800)( unsigned int, int );
extern float(*AGKCommand1801)( unsigned int );
extern float(*AGKCommand1802)( unsigned int );
extern float(*AGKCommand1803)( unsigned int );
extern int(*AGKCommand1804)( unsigned int );
extern int(*AGKCommand1805)( unsigned int );
extern float(*AGKCommand1806)( unsigned int );
extern float(*AGKCommand1807)( unsigned int );
extern float(*AGKCommand1808)( unsigned int );
extern float(*AGKCommand1809)( unsigned int );
extern float(*AGKCommand1810)( unsigned int );
extern float(*AGKCommand1811)( unsigned int );
extern float(*AGKCommand1812)( unsigned int );
extern float(*AGKCommand1813)( unsigned int );
extern int(*AGKCommand1814)( unsigned int );
extern void(*AGKCommand1815)( unsigned int, float, float, float, float, float );
extern void(*AGKCommand1816)( unsigned int );
extern void(*AGKCommand1817)( unsigned int, float, unsigned int, unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1818)( unsigned int );
extern void(*AGKCommand1819)( unsigned int, float, float );
extern void(*AGKCommand1820)( unsigned int );
extern void(*AGKCommand1821)( unsigned int, float );
extern void(*AGKCommand1822)( unsigned int, float, float, float );
extern void(*AGKCommand1823)( unsigned int, unsigned int, int, int, int, int );
extern int(*AGKCommand1824)( unsigned int, unsigned int );
extern int(*AGKCommand1825)( unsigned int, unsigned int );
extern int(*AGKCommand1826)( unsigned int, unsigned int );
extern int(*AGKCommand1827)( unsigned int, unsigned int );
extern void(*AGKCommand1828)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1829)( unsigned int, unsigned int );
extern void(*AGKCommand1830)( unsigned int, float, float );
extern void(*AGKCommand1831)( unsigned int, unsigned int, float, float );
extern unsigned char*(*AGKCommand1832)( unsigned int );
extern void(*AGKCommand1833)( const char *, int );
extern void(*AGKCommand1834)(  );
extern int(*AGKCommand1835)(  );
extern void(*AGKCommand1836)( unsigned int );
extern void(*AGKCommand1837)( const char * );
extern int(*AGKCommand1838)(  );
extern void(*AGKCommand1839)( const char* );
extern char*(*AGKCommand1840)(  );
extern void(*AGKCommand1841)(  );
extern int(*AGKCommand1842)(  );
extern void(*AGKCommand1843)( const char * );
extern void(*AGKCommand1844)( float );
extern void(*AGKCommand1845)( const char* );
extern int(*AGKCommand1846)(  );
extern void(*AGKCommand1847)(  );
extern int(*AGKCommand1848)( unsigned int );
extern void(*AGKCommand1849)( unsigned int, int, int );
extern void(*AGKCommand1850)( unsigned int, float, float, float, float, float, int );
extern void(*AGKCommand1851)( unsigned int, float, float, float, int );
extern void(*AGKCommand1852)( unsigned int, unsigned int, unsigned int, float, float, int );
extern void(*AGKCommand1853)( unsigned int, unsigned int, unsigned int, int, float, float, int );
extern void(*AGKCommand1854)( unsigned int, unsigned int, unsigned int, int, float, float );
extern int(*AGKCommand1855)( unsigned int );
extern int(*AGKCommand1856)( unsigned int, int );
extern float(*AGKCommand1857)( unsigned int, int, int );
extern float(*AGKCommand1858)( unsigned int, int, int );
extern float(*AGKCommand1859)( unsigned int );
extern float(*AGKCommand1860)( unsigned int );
extern void(*AGKCommand1861)( unsigned int, float, int );
extern void(*AGKCommand1862)( unsigned int, float, int );
extern void(*AGKCommand1863)( unsigned int, float, int );
extern void(*AGKCommand1864)( unsigned int, int, int );
extern void(*AGKCommand1865)( unsigned int, int, int );
extern void(*AGKCommand1866)( unsigned int, unsigned int, int );
extern void(*AGKCommand1867)( unsigned int, unsigned int, int, int );
extern void(*AGKCommand1868)( unsigned int, unsigned int, int );
extern void(*AGKCommand1869)( unsigned int, unsigned int, int, int );
extern void(*AGKCommand1870)( unsigned int, unsigned int, unsigned int, float, float, float, float, float, int );
extern unsigned int(*AGKCommand1871)( unsigned int, unsigned int, float, float, float, float, float, int );
extern void(*AGKCommand1872)( unsigned int, float );
extern void(*AGKCommand1873)( unsigned int, float, float );
extern void(*AGKCommand1874)( unsigned int );
extern void(*AGKCommand1875)( unsigned int );
extern void(*AGKCommand1876)( unsigned int, unsigned int );
extern void(*AGKCommand1877)( unsigned int, unsigned int );
extern void(*AGKCommand1878)( unsigned int, unsigned int );
extern void(*AGKCommand1879)( unsigned int, unsigned int );
extern void(*AGKCommand1880)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1881)( unsigned int, unsigned int, unsigned int );
extern void(*AGKCommand1882)( unsigned int, unsigned int );
extern void(*AGKCommand1883)( unsigned int, unsigned int );
extern void(*AGKCommand1884)( unsigned int, unsigned int );
extern void(*AGKCommand1885)( unsigned int, unsigned int );
extern int(*AGKCommand1886)( unsigned int );
extern void(*AGKCommand1887)( unsigned int, int, int );
extern int(*AGKCommand1888)( unsigned int );
extern int(*AGKCommand1889)( unsigned int );
extern void(*AGKCommand1890)( const char* );
extern int(*AGKCommand1891)(  );
extern int(*AGKCommand1892)(  );
extern char*(*AGKCommand1893)( const char*, const char* );
extern void(*AGKCommand1894)( const char*, const char* );
extern void(*AGKCommand1895)( const char* );
extern int(*AGKCommand1896)(  );
extern int(*AGKCommand1897)( const char * );
extern int(*AGKCommand1898)( const char * );
extern int(*AGKCommand1899)( const char* );
extern void(*AGKCommand1900)( const char* );
extern void(*AGKCommand1901)( unsigned int );
extern void(*AGKCommand1902)( float, float, float );
extern void(*AGKCommand1903)(  );
extern int(*AGKCommand1904)(  );
extern void(*AGKCommand1905)(  );
extern void(*AGKCommand1906)(  );
extern void(*AGKCommand1907)(  );
extern void(*AGKCommand1908)( int );
extern void(*AGKCommand1909)( int );
extern float(*AGKCommand1910)(  );
extern int(*AGKCommand1911)( float, float );
extern float(*AGKCommand1912)( int );
extern float(*AGKCommand1913)( int );
extern float(*AGKCommand1914)( int );
extern float(*AGKCommand1915)( int );
extern float(*AGKCommand1916)( int );
extern float(*AGKCommand1917)( int );
extern int(*AGKCommand1918)( int );
extern void(*AGKCommand1919)(  );
extern int(*AGKCommand1920)( int );
extern float(*AGKCommand1921)( int );
extern float(*AGKCommand1922)( int );
extern float(*AGKCommand1923)( int );
extern float(*AGKCommand1924)( int );
extern float(*AGKCommand1925)( int );
extern float(*AGKCommand1926)( int );
extern float(*AGKCommand1927)( int );
extern float(*AGKCommand1928)( int );
extern void(*AGKCommand1929)(  );
extern int(*AGKCommand1930)( int );
extern int(*AGKCommand1931)( int );
extern void(*AGKCommand1932)( int, int );
extern int(*AGKCommand1933)( int );
extern void(*AGKCommand1934)( int );
extern void(*AGKCommand1935)( unsigned int );
extern void(*AGKCommand1936)( unsigned int, unsigned int, int, int );
extern void(*AGKCommand1937)( int );
extern void(*AGKCommand1938)( unsigned int, float, float, float );
extern void(*AGKCommand1939)( const char *, int );
extern void(*AGKCommand1940)( int );
extern void(*AGKCommand1941)( const char*, const char* );
extern int(*AGKCommand1942)(  );
extern void(*AGKCommand1943)(  );
extern int(*AGKCommand1944)(  );
extern int(*AGKCommand1945)(  );
extern char*(*AGKCommand1946)( int );
extern char*(*AGKCommand1947)( int );
extern void(*AGKCommand1948)( int );
extern void(*AGKCommand1949)( unsigned int, const char*, const char* );
extern void(*AGKCommand1950)( unsigned int, const char* );
extern int(*AGKCommand1951)( unsigned int );
extern char*(*AGKCommand1952)( int );
extern void(*AGKCommand1953)( const char* );
extern int(*AGKCommand1954)( unsigned int );
extern int(*AGKCommand1955)( unsigned int );
extern int(*AGKCommand1956)( unsigned int );
extern void(*AGKCommand1957)( unsigned int );
extern void(*AGKCommand1958)( unsigned int );
extern void(*AGKCommand1959)( const char* );
extern char*(*AGKCommand1960)(  );
extern void(*AGKCommand1961)( unsigned int, int );
extern int(*AGKCommand1962)(  );
extern int(*AGKCommand1963)( unsigned int );
extern int(*AGKCommand1964)( unsigned int );
extern char*(*AGKCommand1965)(  );
extern int(*AGKCommand1966)(  );
extern int(*AGKCommand1967)( unsigned int, unsigned int );
extern int(*AGKCommand1968)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand1969)( unsigned int );
extern char*(*AGKCommand1970)(  );
extern void(*AGKCommand1971)( unsigned int, int );
extern int(*AGKCommand1972)( int );
extern int(*AGKCommand1973)( int );
extern int(*AGKCommand1974)( int );
extern int(*AGKCommand1975)( int );
extern void(*AGKCommand1976)(  );
extern void(*AGKCommand1977)( const char*, const char*, const char* );
extern unsigned int(*AGKCommand1978)( const char*, float, float, float, int, int, int, int );
extern void(*AGKCommand1979)( unsigned int, const char*, float, float, float, int, int, int, int );
extern void(*AGKCommand1980)( unsigned int, int, int );
extern void(*AGKCommand1981)( unsigned int, unsigned int, int );
extern char*(*AGKCommand1982)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand1983)( unsigned int );
extern char*(*AGKCommand1984)( unsigned int );
extern void(*AGKCommand1985)( const char*, const char*, float );
extern void(*AGKCommand1986)( const char* );
extern int(*AGKCommand1987)( unsigned int );
extern unsigned int(*AGKCommand1988)( const char*, int );
extern int(*AGKCommand1989)( unsigned int, const char*, int );
extern void(*AGKCommand1990)( unsigned int, unsigned int, const char*, int );
extern unsigned int(*AGKCommand1991)( unsigned int );
extern void(*AGKCommand1992)( unsigned int );
extern void(*AGKCommand1993)( const char*, const char*, const char* );
extern float(*AGKCommand1994)(  );
extern int(*AGKCommand1995)(  );
extern int(*AGKCommand1996)(  );
extern int(*AGKCommand1997)(  );
extern void(*AGKCommand1998)(  );
extern void(*AGKCommand1999)(  );
extern unsigned int(*AGKCommand2000)( const char* );
extern void(*AGKCommand2001)( unsigned int, unsigned int, int );
extern void(*AGKCommand2002)( unsigned int, const char *, unsigned int, float );
extern void(*AGKCommand2003)( unsigned int, const char *, unsigned int, float, float );
extern void(*AGKCommand2004)( unsigned int, const char *, unsigned int, float, float, float );
extern void(*AGKCommand2005)( unsigned int, const char *, unsigned int, float, float, float, float );
extern void(*AGKCommand2006)( unsigned int );
extern void(*AGKCommand2007)( unsigned int, const char *, unsigned int, float );
extern void(*AGKCommand2008)( unsigned int, const char *, unsigned int, float, float );
extern void(*AGKCommand2009)( unsigned int, const char *, unsigned int, float, float, float );
extern void(*AGKCommand2010)( unsigned int, const char *, unsigned int, float, float, float, float );
extern float(*AGKCommand2011)( unsigned int );
extern int(*AGKCommand2012)(  );
extern int(*AGKCommand2013)(  );
extern int(*AGKCommand2014)(  );
extern float(*AGKCommand2015)(  );
extern unsigned int(*AGKCommand2016)( unsigned int, unsigned int, unsigned int, unsigned int );
extern unsigned int(*AGKCommand2017)( unsigned int );
extern void(*AGKCommand2018)( int );
extern void(*AGKCommand2019)(  );
extern int(*AGKCommand2020)( const char * );
extern void(*AGKCommand2021)( float, float, int, int, float );
extern void(*AGKCommand2022)( const char*, const char*, const char*, const char* );
extern void(*AGKCommand2023)( int, int, const char *, const char * );
extern void(*AGKCommand2024)( const char* );
extern void(*AGKCommand2025)( const char* );
extern char*(*AGKCommand2026)(  );
extern unsigned int(*AGKCommand2027)( unsigned int );
extern void*(*AGKCommand2028)( unsigned int );
extern void(*AGKCommand2029)( unsigned int, int );
extern char*(*AGKCommand2030)( unsigned int, unsigned int );
extern char*(*AGKCommand2031)( unsigned int );
extern int(*AGKCommand2032)(  );
extern int(*AGKCommand2033)(  );
extern int(*AGKCommand2034)(  );
extern int(*AGKCommand2035)(  );
extern int(*AGKCommand2036)(  );
extern int(*AGKCommand2037)(  );
extern void(*AGKCommand2038)( int );
extern void(*AGKCommand2039)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand2040)( unsigned int );
extern float(*AGKCommand2041)( unsigned int );
extern float(*AGKCommand2042)( unsigned int );
extern float(*AGKCommand2043)( unsigned int );
extern void(*AGKCommand2044)( unsigned int, unsigned int );
extern void(*AGKCommand2045)(  );
extern void(*AGKCommand2046)( unsigned int, int, int );
extern void(*AGKCommand2047)( int );
extern int(*AGKCommand2048)(  );
extern char*(*AGKCommand2049)( unsigned int );
extern char*(*AGKCommand2050)( unsigned int );
extern void(*AGKCommand2051)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand2052)( unsigned int );
extern void(*AGKCommand2053)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand2054)( unsigned int );
extern void(*AGKCommand2055)( unsigned int, unsigned int );
extern unsigned int(*AGKCommand2056)( unsigned int );
extern void(*AGKCommand2057)( unsigned int, const char* );
extern int(*AGKCommand2058)( int );
extern void(*AGKCommand2059)( const char* );
extern char*(*AGKCommand2060)( int );
extern void(*AGKCommand2061)( unsigned int, unsigned int );
extern float(*AGKCommand2062)( unsigned int );
extern float(*AGKCommand2063)( unsigned int );
extern float(*AGKCommand2064)( unsigned int );
extern float(*AGKCommand2065)( unsigned int );
extern float(*AGKCommand2066)( unsigned int );
extern float(*AGKCommand2067)( unsigned int );
extern float(*AGKCommand2068)( unsigned int );
extern int(*AGKCommand2069)(  );
extern void(*AGKCommand2070)( unsigned int, float );
extern float(*AGKCommand2071)( unsigned int );
extern void(*AGKCommand2072)( unsigned int, int );
extern void(*AGKCommand2073)( unsigned int, int );
extern float(*AGKCommand2074)( unsigned int );
extern int(*AGKCommand2075)( unsigned int );
extern int(*AGKCommand2076)( unsigned int );
extern float(*AGKCommand2077)( unsigned int );
extern float(*AGKCommand2078)( unsigned int );
extern void(*AGKCommand2079)( unsigned int );
extern void(*AGKCommand2080)( unsigned int );
extern void(*AGKCommand2081)( unsigned int );
extern void(*AGKCommand2082)( unsigned int );
extern void(*AGKCommand2083)( unsigned int );
extern void(*AGKCommand2084)( unsigned int );
extern float(*AGKCommand2085)( unsigned int, unsigned int );
extern float(*AGKCommand2086)( unsigned int, unsigned int );
extern float(*AGKCommand2087)( unsigned int, unsigned int );
extern float(*AGKCommand2088)( unsigned int, unsigned int );
extern float(*AGKCommand2089)( unsigned int, unsigned int, unsigned int );
extern float(*AGKCommand2090)( unsigned int, unsigned int, unsigned int );
extern float(*AGKCommand2091)( unsigned int, unsigned int );
extern float(*AGKCommand2092)( unsigned int, unsigned int );
extern void(*AGKCommand2093)( float, float, float, float );
extern float(*AGKCommand2094)( float );
extern float(*AGKCommand2095)( float, float );
extern float(*AGKCommand2096)( float, float, float );
extern float(*AGKCommand2097)( unsigned int, float );
extern float(*AGKCommand2098)( unsigned int, float, float );
extern float(*AGKCommand2099)( unsigned int, float, float, float );
extern void(*AGKCommand2100)( unsigned int, unsigned int );
extern void(*AGKCommand2101)( unsigned int, const char*, float, float, float, float );
extern void(*AGKCommand2102)( unsigned int, const char*, unsigned int, float, float, float, float );
extern void(*AGKCommand2103)( unsigned int, const char* );
extern void(*AGKCommand2104)(  );
extern int(*AGKCommand2105)( const char* );
extern void(*AGKCommand2106)( const char*, const char*, const char*, const char* );
extern int(*AGKCommand2107)( const char*, const char*, const char*, const char* );
extern float(*AGKCommand2108)( const char*, const char*, const char*, const char* );
extern char*(*AGKCommand2109)( const char*, const char*, const char*, const char* );
extern char*(*AGKCommand2110)(  );
extern void(*AGKCommand2111)( int, const char* );
extern int(*AGKCommand2112)( int );
extern int(*AGKCommand2113)( int, int );
extern char*(*AGKCommand2114)( int, int, int );
extern int(*AGKCommand2115)( int, int, int );
extern char*(*AGKCommand2116)( int, int, int );
extern int(*AGKCommand2117)( int, int, int );
extern char*(*AGKCommand2118)( int, int );
extern char*(*AGKCommand2119)( int, int );
extern int(*AGKCommand2120)(  );
extern float(*AGKCommand2121)( int );
extern float(*AGKCommand2122)( int );
extern float(*AGKCommand2123)( int );
extern float(*AGKCommand2124)( int );
extern float(*AGKCommand2125)(  );
extern float(*AGKCommand2126)(  );
extern float(*AGKCommand2127)(  );
extern float(*AGKCommand2128)(  );
extern int(*AGKCommand2129)(  );
extern void(*AGKCommand2130)( int );
extern int(*AGKCommand2131)(  );
extern int(*AGKCommand2132)( int );

class agk
{
	public:
		static inline char* CreateString( unsigned int size ) { return AGKCommand0( size ); }
		static inline void DeleteString( char* str ) { AGKCommand1( str ); }
		static inline float WorldToScreenX( float x ) { return AGKCommand2( x ); }
		static inline float WorldToScreenY( float y ) { return AGKCommand3( y ); }
		static inline float ScreenToWorldX( float x ) { return AGKCommand4( x ); }
		static inline float ScreenToWorldY( float y ) { return AGKCommand5( y ); }
		static inline void SetResolutionMode( int mode ) { AGKCommand6( mode ); }
		static inline void SetSyncRate( float fps, int mode ) { AGKCommand7( fps, mode ); }
		static inline void SetOrientationAllowed( int portrait, int portrait2, int landscape, int landscape2 ) { AGKCommand8( portrait, portrait2, landscape, landscape2 ); }
		static inline void SetTransitionMode( int mode ) { AGKCommand9( mode ); }
		static inline void SetDisplayAspect( float aspect ) { AGKCommand10( aspect ); }
		static inline void SetRenderToImage( int colorImage, int depthImage ) { AGKCommand11( colorImage, depthImage ); }
		static inline void SetRenderToScreen(  ) { AGKCommand12(  ); }
		static inline char* GetCL(  ) { return AGKCommand13(  ); }
		static inline void SetScissor( float x, float y, float x2, float y2 ) { AGKCommand14( x, y, x2, y2 ); }
		static inline void SetIntendedDeviceSize( int width, int height ) { AGKCommand15( width, height ); }
		static inline void UseNewDefaultFonts( int mode ) { AGKCommand16( mode ); }
		static inline int GetDeviceWidth(  ) { return AGKCommand17(  ); }
		static inline int GetDeviceHeight(  ) { return AGKCommand18(  ); }
		static inline void SetVirtualResolution( int iWidth, int iHeight ) { AGKCommand19( iWidth, iHeight ); }
		static inline void UpdateDeviceSize( unsigned int w, unsigned int h ) { AGKCommand20( w, h ); }
		static inline int GetVirtualWidth(  ) { return AGKCommand21(  ); }
		static inline int GetVirtualHeight(  ) { return AGKCommand22(  ); }
		static inline float GetScreenBoundsLeft(  ) { return AGKCommand23(  ); }
		static inline float GetScreenBoundsRight(  ) { return AGKCommand24(  ); }
		static inline float GetScreenBoundsTop(  ) { return AGKCommand25(  ); }
		static inline float GetScreenBoundsBottom(  ) { return AGKCommand26(  ); }
		static inline float GetDisplayAspect(  ) { return AGKCommand27(  ); }
		static inline int GetOrientation(  ) { return AGKCommand28(  ); }
		static inline int GetPaused(  ) { return AGKCommand29(  ); }
		static inline int GetResumed(  ) { return AGKCommand30(  ); }
		static inline void SetDefaultMagFilter( unsigned int filter ) { AGKCommand31( filter ); }
		static inline void SetDefaultMinFilter( unsigned int filter ) { AGKCommand32( filter ); }
		static inline void SetDefaultWrapU( unsigned int mode ) { AGKCommand33( mode ); }
		static inline void SetDefaultWrapV( unsigned int mode ) { AGKCommand34( mode ); }
		static inline void SetViewOffset( float x, float y ) { AGKCommand35( x, y ); }
		static inline void SetViewZoom( float zoom ) { AGKCommand36( zoom ); }
		static inline void SetViewZoomMode( int mode ) { AGKCommand37( mode ); }
		static inline float GetViewOffsetX(  ) { return AGKCommand38(  ); }
		static inline float GetViewOffsetY(  ) { return AGKCommand39(  ); }
		static inline float GetViewZoom(  ) { return AGKCommand40(  ); }
		static inline void Sync(  ) { AGKCommand41(  ); }
		static inline void Break(  ) { AGKCommand42(  ); }
		static inline void StepPhysics( float time ) { AGKCommand43( time ); }
		static inline void Update( float time ) { AGKCommand44( time ); }
		static inline void Update2D( float time ) { AGKCommand45( time ); }
		static inline void Update3D( float time ) { AGKCommand46( time ); }
		static inline void Render2DBack(  ) { AGKCommand47(  ); }
		static inline void Render(  ) { AGKCommand48(  ); }
		static inline void Render2DFront(  ) { AGKCommand49(  ); }
		static inline void Render3D(  ) { AGKCommand50(  ); }
		static inline void Swap(  ) { AGKCommand51(  ); }
		static inline void ClearScreen(  ) { AGKCommand52(  ); }
		static inline void ClearDepthBuffer(  ) { AGKCommand53(  ); }
		static inline void SetGenerateMipmaps( int generate ) { AGKCommand54( generate ); }
		static inline float Timer(  ) { return AGKCommand55(  ); }
		static inline void ResetTimer(  ) { AGKCommand56(  ); }
		static inline float GetRunTime(  ) { return AGKCommand57(  ); }
		static inline int GetSeconds(  ) { return AGKCommand58(  ); }
		static inline float GetFrameTime(  ) { return AGKCommand59(  ); }
		static inline unsigned int GetMilliseconds(  ) { return AGKCommand60(  ); }
		static inline char* GetDeviceID(  ) { return AGKCommand61(  ); }
		static inline int GetNumProcessors(  ) { return AGKCommand62(  ); }
		static inline char* Sha1( const char* str ) { return AGKCommand63( str ); }
		static inline char* Sha256( const char* str ) { return AGKCommand64( str ); }
		static inline char* Sha512( const char* str ) { return AGKCommand65( str ); }
		static inline char* HTTPEncode( const char* str ) { return AGKCommand66( str ); }
		static inline char* HTTPDecode( const char* str ) { return AGKCommand67( str ); }
		static inline void SetRandomSeed( unsigned int seed ) { AGKCommand68( seed ); }
		static inline unsigned int Random(  ) { return AGKCommand69(  ); }
		static inline unsigned int Random( unsigned int from, unsigned int to ) { return AGKCommand70( from, to ); }
		static inline int RandomSign( int value ) { return AGKCommand71( value ); }
		static inline void SetRandomSeed2( unsigned int seed ) { AGKCommand72( seed ); }
		static inline int Random2(  ) { return AGKCommand73(  ); }
		static inline int Random2( int from, int to ) { return AGKCommand74( from, to ); }
		static inline float ATanFull( float x, float y ) { return AGKCommand75( x, y ); }
		static inline float ATanFullRad( float x, float y ) { return AGKCommand76( x, y ); }
		static inline void SetSortTextures( int sort ) { AGKCommand77( sort ); }
		static inline void SetSortDepth( int sort ) { AGKCommand78( sort ); }
		static inline void SetSortTransparentDepth( int sort ) { AGKCommand79( sort ); }
		static inline void SetSortCreated( int sort ) { AGKCommand80( sort ); }
		static inline float ScreenFPS(  ) { return AGKCommand81(  ); }
		static inline unsigned int GetPolygonsDrawn(  ) { return AGKCommand82(  ); }
		static inline unsigned int GetVerticesProcessed(  ) { return AGKCommand83(  ); }
		static inline void EnableClearDepth( unsigned int clear ) { AGKCommand84( clear ); }
		static inline void EnableClearColor( unsigned int clear ) { AGKCommand85( clear ); }
		static inline void SetBorderColor( unsigned int red, unsigned int green, unsigned int blue ) { AGKCommand86( red, green, blue ); }
		static inline void SetClearColor( unsigned int red, unsigned int green, unsigned int blue ) { AGKCommand87( red, green, blue ); }
		static inline cImage* GetImagePtr( unsigned int iImageIndex ) { return AGKCommand88( iImageIndex ); }
		static inline unsigned int GetImageTextureID( unsigned int iImageIndex ) { return AGKCommand89( iImageIndex ); }
		static inline void LoadImage( unsigned int iImageIndex, const char * sImageFilename, int bBlackToAlpha ) { AGKCommand90( iImageIndex, sImageFilename, bBlackToAlpha ); }
		static inline void LoadImage( unsigned int ID, const char* sImageFilename ) { AGKCommand91( ID, sImageFilename ); }
		static inline unsigned int LoadImage( const char* sImageFilename, int bBlackToAlpha ) { return AGKCommand92( sImageFilename, bBlackToAlpha ); }
		static inline unsigned int LoadImage( const char* sImageFilename ) { return AGKCommand93( sImageFilename ); }
		static inline void LoadSubImage( unsigned int iImageIndex, unsigned int iParentIndex, const char * sImageFilename ) { AGKCommand94( iImageIndex, iParentIndex, sImageFilename ); }
		static inline unsigned int LoadSubImage( unsigned int iParentIndex, const char * sImageFilename ) { return AGKCommand95( iParentIndex, sImageFilename ); }
		static inline void LoadImageResized( unsigned int iImageID, const char* szFilename, float scaleX, float scaleY, int cache ) { AGKCommand96( iImageID, szFilename, scaleX, scaleY, cache ); }
		static inline unsigned int LoadImageResized( const char* szFilename, float scaleX, float scaleY, int cache ) { return AGKCommand97( szFilename, scaleX, scaleY, cache ); }
		static inline void CreateImageColor( unsigned int imageID, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha ) { AGKCommand98( imageID, red, green, blue, alpha ); }
		static inline unsigned int CreateImageColor( unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha ) { return AGKCommand99( red, green, blue, alpha ); }
		static inline void CreateRenderImage( unsigned int imageID, unsigned int width, unsigned int height, unsigned int format, unsigned int mipmap ) { AGKCommand100( imageID, width, height, format, mipmap ); }
		static inline unsigned int CreateRenderImage( unsigned int width, unsigned int height, unsigned int format, unsigned int mipmap ) { return AGKCommand101( width, height, format, mipmap ); }
		static inline unsigned int GetImageExists( unsigned int iImageIndex ) { return AGKCommand102( iImageIndex ); }
		static inline void DeleteImage( unsigned int iImageIndex ) { AGKCommand103( iImageIndex ); }
		static inline void DeleteAllImages(  ) { AGKCommand104(  ); }
		static inline float GetImageWidth( unsigned int iImageIndex ) { return AGKCommand105( iImageIndex ); }
		static inline float GetImageHeight( unsigned int iImageIndex ) { return AGKCommand106( iImageIndex ); }
		static inline void SetImageMinFilter( unsigned int iImageIndex, unsigned int mode ) { AGKCommand107( iImageIndex, mode ); }
		static inline void SetImageMagFilter( unsigned int iImageIndex, unsigned int mode ) { AGKCommand108( iImageIndex, mode ); }
		static inline void SetImageWrapU( unsigned int iImageIndex, unsigned int mode ) { AGKCommand109( iImageIndex, mode ); }
		static inline void SetImageWrapV( unsigned int iImageIndex, unsigned int mode ) { AGKCommand110( iImageIndex, mode ); }
		static inline void SetImageMask( unsigned int iDstImage, unsigned int iSrcImage, int dst, int src, int x, int y ) { AGKCommand111( iDstImage, iSrcImage, dst, src, x, y ); }
		static inline void SetImageTransparentColor( unsigned int iImage, int r, int g, int b ) { AGKCommand112( iImage, r, g, b ); }
		static inline void SaveImage( unsigned int iImageIndex, const char* filename ) { AGKCommand113( iImageIndex, filename ); }
		static inline unsigned int GetImage( float x, float y, float width, float height ) { return AGKCommand114( x, y, width, height ); }
		static inline void GetImage( unsigned int imageID, float x, float y, float width, float height ) { AGKCommand115( imageID, x, y, width, height ); }
		static inline char* GetImageFilename( unsigned int imageID ) { return AGKCommand116( imageID ); }
		static inline unsigned int CopyImage( unsigned int fromImage, int x, int y, int width, int height ) { return AGKCommand117( fromImage, x, y, width, height ); }
		static inline void CopyImage( unsigned int newImage, unsigned int fromImage, int x, int y, int width, int height ) { AGKCommand118( newImage, fromImage, x, y, width, height ); }
		static inline void ResizeImage( unsigned int imageID, int width, int height ) { AGKCommand119( imageID, width, height ); }
		static inline unsigned int ShowChooseImageScreen(  ) { return AGKCommand120(  ); }
		static inline unsigned int IsChoosingImage(  ) { return AGKCommand121(  ); }
		static inline unsigned int GetChosenImage(  ) { return AGKCommand122(  ); }
		static inline unsigned int ShowImageCaptureScreen(  ) { return AGKCommand123(  ); }
		static inline unsigned int IsCapturingImage(  ) { return AGKCommand124(  ); }
		static inline unsigned int GetCapturedImage(  ) { return AGKCommand125(  ); }
		static inline char* DecodeQRCode( unsigned int image ) { return AGKCommand126( image ); }
		static inline unsigned int EncodeQRCode( const char* text, int errormode ) { return AGKCommand127( text, errormode ); }
		static inline void PrintImage( unsigned int image, float size ) { AGKCommand128( image, size ); }
		static inline void SetImageSavePixels( int mode ) { AGKCommand129( mode ); }
		static inline cSprite* GetSpritePtr( unsigned int iSpriteIndex ) { return AGKCommand130( iSpriteIndex ); }
		static inline unsigned int GetSpriteImageID( unsigned int iSpriteIndex ) { return AGKCommand131( iSpriteIndex ); }
		static inline void CreateSprite( unsigned int iSpriteIndex, unsigned int iImageIndex ) { AGKCommand132( iSpriteIndex, iImageIndex ); }
		static inline unsigned int CreateSprite( unsigned int iImageIndex ) { return AGKCommand133( iImageIndex ); }
		static inline unsigned int LoadSprite( const char* imagefile ) { return AGKCommand134( imagefile ); }
		static inline void LoadSprite( unsigned int iSpriteIndex, const char* imagefile ) { AGKCommand135( iSpriteIndex, imagefile ); }
		static inline void CloneSprite( unsigned int iSpriteIndex, unsigned int iOtherSprite ) { AGKCommand136( iSpriteIndex, iOtherSprite ); }
		static inline unsigned int CloneSprite( unsigned int iOtherSprite ) { return AGKCommand137( iOtherSprite ); }
		static inline void CreateDummySprite( unsigned int iSpriteIndex ) { AGKCommand138( iSpriteIndex ); }
		static inline unsigned int CreateDummySprite(  ) { return AGKCommand139(  ); }
		static inline unsigned int GetSpriteExists( unsigned int iSpriteIndex ) { return AGKCommand140( iSpriteIndex ); }
		static inline void DeleteSprite( unsigned int iSpriteIndex ) { AGKCommand141( iSpriteIndex ); }
		static inline void DeleteAllSprites(  ) { AGKCommand142(  ); }
		static inline void SetSpriteImage( unsigned int iSpriteIndex, unsigned int iImageIndex ) { AGKCommand143( iSpriteIndex, iImageIndex ); }
		static inline void SetSpriteImage( unsigned int iSpriteIndex, unsigned int iImageIndex, int bUpdateShape ) { AGKCommand144( iSpriteIndex, iImageIndex, bUpdateShape ); }
		static inline void SetSpriteAdditionalImage( unsigned int iSpriteIndex, unsigned int iImageIndex, int iStage ) { AGKCommand145( iSpriteIndex, iImageIndex, iStage ); }
		static inline void SetSpriteUV( unsigned int iSpriteIndex, float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4 ) { AGKCommand146( iSpriteIndex, u1, v1, u2, v2, u3, v3, u4, v4 ); }
		static inline void ResetSpriteUV( unsigned int iSpriteIndex ) { AGKCommand147( iSpriteIndex ); }
		static inline void SetSpritePosition( unsigned int iSpriteIndex, float fX, float fY ) { AGKCommand148( iSpriteIndex, fX, fY ); }
		static inline void SetSpritePositionByOffset( unsigned int iSpriteIndex, float fX, float fY ) { AGKCommand149( iSpriteIndex, fX, fY ); }
		static inline void SetSpriteX( unsigned int iSpriteIndex, float fX ) { AGKCommand150( iSpriteIndex, fX ); }
		static inline void SetSpriteY( unsigned int iSpriteIndex, float fY ) { AGKCommand151( iSpriteIndex, fY ); }
		static inline void SetSpriteAngle( unsigned int iSpriteIndex, float fAng ) { AGKCommand152( iSpriteIndex, fAng ); }
		static inline void SetSpriteAngleRad( unsigned int iSpriteIndex, float fAng ) { AGKCommand153( iSpriteIndex, fAng ); }
		static inline void SetSpriteDepth( unsigned int iSpriteIndex, int iDepth ) { AGKCommand154( iSpriteIndex, iDepth ); }
		static inline void SetSpriteFlip( unsigned int iSpriteIndex, int horz, int vert ) { AGKCommand155( iSpriteIndex, horz, vert ); }
		static inline void SetSpriteSnap( unsigned int iSpriteIndex, int snap ) { AGKCommand156( iSpriteIndex, snap ); }
		static inline void SetSpriteVisible( unsigned int iSpriteIndex, int bVisible ) { AGKCommand157( iSpriteIndex, bVisible ); }
		static inline void SetSpriteActive( unsigned int iSpriteIndex, int bActive ) { AGKCommand158( iSpriteIndex, bActive ); }
		static inline float GetSpriteX( unsigned int iSpriteIndex ) { return AGKCommand159( iSpriteIndex ); }
		static inline float GetSpriteY( unsigned int iSpriteIndex ) { return AGKCommand160( iSpriteIndex ); }
		static inline float GetSpriteXByOffset( unsigned int iSpriteIndex ) { return AGKCommand161( iSpriteIndex ); }
		static inline float GetSpriteYByOffset( unsigned int iSpriteIndex ) { return AGKCommand162( iSpriteIndex ); }
		static inline float GetSpriteOffsetX( unsigned int iSpriteIndex ) { return AGKCommand163( iSpriteIndex ); }
		static inline float GetSpriteOffsetY( unsigned int iSpriteIndex ) { return AGKCommand164( iSpriteIndex ); }
		static inline float GetSpriteWidth( unsigned int iSpriteIndex ) { return AGKCommand165( iSpriteIndex ); }
		static inline float GetSpriteAngle( unsigned int iSpriteIndex ) { return AGKCommand166( iSpriteIndex ); }
		static inline float GetSpriteAngleRad( unsigned int iSpriteIndex ) { return AGKCommand167( iSpriteIndex ); }
		static inline float GetSpriteHeight( unsigned int iSpriteIndex ) { return AGKCommand168( iSpriteIndex ); }
		static inline float GetSpriteScaleX( unsigned int iSpriteIndex ) { return AGKCommand169( iSpriteIndex ); }
		static inline float GetSpriteScaleY( unsigned int iSpriteIndex ) { return AGKCommand170( iSpriteIndex ); }
		static inline int GetSpriteHitTest( unsigned int iSpriteIndex, float x, float y ) { return AGKCommand171( iSpriteIndex, x, y ); }
		static inline int GetSpriteHit( float x, float y ) { return AGKCommand172( x, y ); }
		static inline int GetSpriteHitGroup( int group, float x, float y ) { return AGKCommand173( group, x, y ); }
		static inline int GetSpriteHitCategory( unsigned int categories, float x, float y ) { return AGKCommand174( categories, x, y ); }
		static inline int GetSpriteDepth( unsigned int iSpriteIndex ) { return AGKCommand175( iSpriteIndex ); }
		static inline unsigned int GetSpriteColorRed( unsigned int iSpriteIndex ) { return AGKCommand176( iSpriteIndex ); }
		static inline unsigned int GetSpriteColorGreen( unsigned int iSpriteIndex ) { return AGKCommand177( iSpriteIndex ); }
		static inline unsigned int GetSpriteColorBlue( unsigned int iSpriteIndex ) { return AGKCommand178( iSpriteIndex ); }
		static inline unsigned int GetSpriteColorAlpha( unsigned int iSpriteIndex ) { return AGKCommand179( iSpriteIndex ); }
		static inline int GetSpritePlaying( unsigned int iSpriteIndex ) { return AGKCommand180( iSpriteIndex ); }
		static inline int GetSpriteVisible( unsigned int iSpriteIndex ) { return AGKCommand181( iSpriteIndex ); }
		static inline int GetSpriteActive( unsigned int iSpriteIndex ) { return AGKCommand182( iSpriteIndex ); }
		static inline int GetSpriteGroup( unsigned int iSpriteIndex ) { return AGKCommand183( iSpriteIndex ); }
		static inline float GetSpriteXFromPixel( unsigned int iSpriteIndex, int x ) { return AGKCommand184( iSpriteIndex, x ); }
		static inline float GetSpriteYFromPixel( unsigned int iSpriteIndex, int y ) { return AGKCommand185( iSpriteIndex, y ); }
		static inline int GetSpritePixelFromX( unsigned int iSpriteIndex, float x ) { return AGKCommand186( iSpriteIndex, x ); }
		static inline int GetSpritePixelFromY( unsigned int iSpriteIndex, float y ) { return AGKCommand187( iSpriteIndex, y ); }
		static inline float GetWorldXFromSprite( unsigned int iSpriteIndex, float x, float y ) { return AGKCommand188( iSpriteIndex, x, y ); }
		static inline float GetWorldYFromSprite( unsigned int iSpriteIndex, float x, float y ) { return AGKCommand189( iSpriteIndex, x, y ); }
		static inline float GetSpriteXFromWorld( unsigned int iSpriteIndex, float x, float y ) { return AGKCommand190( iSpriteIndex, x, y ); }
		static inline float GetSpriteYFromWorld( unsigned int iSpriteIndex, float x, float y ) { return AGKCommand191( iSpriteIndex, x, y ); }
		static inline void SetSpriteSize( unsigned int iSpriteIndex, float width, float height ) { AGKCommand192( iSpriteIndex, width, height ); }
		static inline void SetSpriteAnimation( unsigned int iSpriteIndex, int iFrameWidth, int iFrameHeight, int iFrameCount ) { AGKCommand193( iSpriteIndex, iFrameWidth, iFrameHeight, iFrameCount ); }
		static inline void AddSpriteAnimationFrame( unsigned int iSpriteIndex, unsigned int iImageIndex ) { AGKCommand194( iSpriteIndex, iImageIndex ); }
		static inline void ClearSpriteAnimationFrames( unsigned int iSpriteIndex ) { AGKCommand195( iSpriteIndex ); }
		static inline void SetSpriteSpeed( unsigned int iSpriteIndex, float fFps ) { AGKCommand196( iSpriteIndex, fFps ); }
		static inline void PlaySprite( unsigned int iSpriteIndex ) { AGKCommand197( iSpriteIndex ); }
		static inline void PlaySprite( unsigned int iSpriteIndex, float fFps ) { AGKCommand198( iSpriteIndex, fFps ); }
		static inline void PlaySprite( unsigned int iSpriteIndex, float fFps, int iLoop ) { AGKCommand199( iSpriteIndex, fFps, iLoop ); }
		static inline void PlaySprite( unsigned int iSpriteIndex, float fFps, int iLoop, int iFromFrame, int iToFrame ) { AGKCommand200( iSpriteIndex, fFps, iLoop, iFromFrame, iToFrame ); }
		static inline void StopSprite( unsigned int iSpriteIndex ) { AGKCommand201( iSpriteIndex ); }
		static inline void ResumeSprite( unsigned int iSpriteIndex ) { AGKCommand202( iSpriteIndex ); }
		static inline void SetSpriteFrame( unsigned int iSpriteIndex, int iFrame ) { AGKCommand203( iSpriteIndex, iFrame ); }
		static inline int GetSpriteCurrentFrame( unsigned int iSpriteIndex ) { return AGKCommand204( iSpriteIndex ); }
		static inline int GetSpriteFrameCount( unsigned int iSpriteIndex ) { return AGKCommand205( iSpriteIndex ); }
		static inline void SetSpriteTransparency( unsigned int iSpriteIndex, int mode ) { AGKCommand206( iSpriteIndex, mode ); }
		static inline void SetSpriteOffset( unsigned int iSpriteIndex, float x, float y ) { AGKCommand207( iSpriteIndex, x, y ); }
		static inline void SetSpriteColor( unsigned int iSpriteIndex, int iRed, int iGreen, int iBlue, int iAlpha ) { AGKCommand208( iSpriteIndex, iRed, iGreen, iBlue, iAlpha ); }
		static inline void SetSpriteColorRed( unsigned int iSpriteIndex, int iRed ) { AGKCommand209( iSpriteIndex, iRed ); }
		static inline void SetSpriteColorGreen( unsigned int iSpriteIndex, int iGreen ) { AGKCommand210( iSpriteIndex, iGreen ); }
		static inline void SetSpriteColorBlue( unsigned int iSpriteIndex, int iBlue ) { AGKCommand211( iSpriteIndex, iBlue ); }
		static inline void SetSpriteColorAlpha( unsigned int iSpriteIndex, int iAlpha ) { AGKCommand212( iSpriteIndex, iAlpha ); }
		static inline void SetSpriteUVBorder( unsigned int iSpriteIndex, float border ) { AGKCommand213( iSpriteIndex, border ); }
		static inline void SetSpriteUVOffset( unsigned int iSpriteIndex, float u, float v ) { AGKCommand214( iSpriteIndex, u, v ); }
		static inline void SetSpriteUVScale( unsigned int iSpriteIndex, float scaleU, float scaleV ) { AGKCommand215( iSpriteIndex, scaleU, scaleV ); }
		static inline void FixSpriteToScreen( unsigned int iSpriteIndex, int mode ) { AGKCommand216( iSpriteIndex, mode ); }
		static inline void SetSpriteScale( unsigned int iSpriteIndex, float x, float y ) { AGKCommand217( iSpriteIndex, x, y ); }
		static inline void SetSpriteScaleByOffset( unsigned int iSpriteIndex, float x, float y ) { AGKCommand218( iSpriteIndex, x, y ); }
		static inline void SetSpriteScissor( unsigned int iSpriteIndex, float x, float y, float x2, float y2 ) { AGKCommand219( iSpriteIndex, x, y, x2, y2 ); }
		static inline void SetSpriteShader( unsigned int spriteID, unsigned int shaderID ) { AGKCommand220( spriteID, shaderID ); }
		static inline void DrawSprite( unsigned int iSpriteIndex ) { AGKCommand221( iSpriteIndex ); }
		static inline void SetPhysicsScale( float scale ) { AGKCommand222( scale ); }
		static inline void SetPhysicsGravity( float x, float y ) { AGKCommand223( x, y ); }
		static inline void SetPhysicsDebugOn(  ) { AGKCommand224(  ); }
		static inline void SetPhysicsDebugOff(  ) { AGKCommand225(  ); }
		static inline void SetPhysicsThreading( int threads ) { AGKCommand226( threads ); }
		static inline void SetPhysicsCCD( int mode ) { AGKCommand227( mode ); }
		static inline void SetPhysicsSleeping( int mode ) { AGKCommand228( mode ); }
		static inline float GetPhysicsSolveTime(  ) { return AGKCommand229(  ); }
		static inline int GetPhysicsIslandCount(  ) { return AGKCommand230(  ); }
		static inline void SetPhysicsMaxPolygonPoints( int points ) { AGKCommand231( points ); }
		static inline void SetPhysicsWallTop( int mode ) { AGKCommand232( mode ); }
		static inline void SetPhysicsWallLeft( int mode ) { AGKCommand233( mode ); }
		static inline void SetPhysicsWallRight( int mode ) { AGKCommand234( mode ); }
		static inline void SetPhysicsWallBottom( int mode ) { AGKCommand235( mode ); }
		static inline unsigned int CreatePhysicsForce( float x, float y, float power, float limit, float range, int fade ) { return AGKCommand236( x, y, power, limit, range, fade ); }
		static inline void DeletePhysicsForce( unsigned int iForceIndex ) { AGKCommand237( iForceIndex ); }
		static inline void SetPhysicsForcePosition( unsigned int iForceIndex, float x, float y ) { AGKCommand238( iForceIndex, x, y ); }
		static inline void SetPhysicsForcePower( unsigned int iForceIndex, float power ) { AGKCommand239( iForceIndex, power ); }
		static inline void SetPhysicsForceRange( unsigned int iForceIndex, float range ) { AGKCommand240( iForceIndex, range ); }
		static inline void SetSpriteShape( unsigned int iSpriteIndex, int shape ) { AGKCommand241( iSpriteIndex, shape ); }
		static inline void SetSpriteShapeBox( unsigned int iSpriteIndex, float x, float y, float x2, float y2, float angle ) { AGKCommand242( iSpriteIndex, x, y, x2, y2, angle ); }
		static inline void SetSpriteShapeCircle( unsigned int iSpriteIndex, float x, float y, float radius ) { AGKCommand243( iSpriteIndex, x, y, radius ); }
		static inline void SetSpriteShapePolygon( unsigned int iSpriteIndex, unsigned int numPoints, unsigned int index, float x, float y ) { AGKCommand244( iSpriteIndex, numPoints, index, x, y ); }
		static inline void AddSpriteShapeBox( unsigned int iSpriteIndex, float x, float y, float x2, float y2, float angle ) { AGKCommand245( iSpriteIndex, x, y, x2, y2, angle ); }
		static inline void AddSpriteShapeCircle( unsigned int iSpriteIndex, float x, float y, float radius ) { AGKCommand246( iSpriteIndex, x, y, radius ); }
		static inline void AddSpriteShapePolygon( unsigned int iSpriteIndex, unsigned int numPoints, unsigned int index, float x, float y ) { AGKCommand247( iSpriteIndex, numPoints, index, x, y ); }
		static inline void ClearSpriteShapes( unsigned int iSpriteIndex ) { AGKCommand248( iSpriteIndex ); }
		static inline void SetSpritePhysicsCOM( unsigned int iSpriteIndex, float x, float y ) { AGKCommand249( iSpriteIndex, x, y ); }
		static inline void CalculateSpritePhysicsCOM( unsigned int iSpriteIndex ) { AGKCommand250( iSpriteIndex ); }
		static inline void SetSpritePhysicsOn( unsigned int iSpriteIndex, int mode ) { AGKCommand251( iSpriteIndex, mode ); }
		static inline void SetSpritePhysicsOff( unsigned int iSpriteIndex ) { AGKCommand252( iSpriteIndex ); }
		static inline void SetSpritePhysicsDelete( unsigned int iSpriteIndex ) { AGKCommand253( iSpriteIndex ); }
		static inline void SetSpritePhysicsFriction( unsigned int iSpriteIndex, float friction ) { AGKCommand254( iSpriteIndex, friction ); }
		static inline void SetSpritePhysicsRestitution( unsigned int iSpriteIndex, float restitution ) { AGKCommand255( iSpriteIndex, restitution ); }
		static inline void SetSpritePhysicsCanRotate( unsigned int iSpriteIndex, int rotate ) { AGKCommand256( iSpriteIndex, rotate ); }
		static inline void SetSpritePhysicsVelocity( unsigned int iSpriteIndex, float vx, float vy ) { AGKCommand257( iSpriteIndex, vx, vy ); }
		static inline void SetSpritePhysicsAngularVelocity( unsigned int iSpriteIndex, float va ) { AGKCommand258( iSpriteIndex, va ); }
		static inline void SetSpritePhysicsDamping( unsigned int iSpriteIndex, float damp ) { AGKCommand259( iSpriteIndex, damp ); }
		static inline void SetSpritePhysicsAngularDamping( unsigned int iSpriteIndex, float damp ) { AGKCommand260( iSpriteIndex, damp ); }
		static inline void SetSpritePhysicsIsBullet( unsigned int iSpriteIndex, int bullet ) { AGKCommand261( iSpriteIndex, bullet ); }
		static inline void SetSpritePhysicsMass( unsigned int iSpriteIndex, float mass ) { AGKCommand262( iSpriteIndex, mass ); }
		static inline void SetSpritePhysicsIsSensor( unsigned int iSpriteIndex, int sensor ) { AGKCommand263( iSpriteIndex, sensor ); }
		static inline void SetSpriteGroup( unsigned int iSpriteIndex, int group ) { AGKCommand264( iSpriteIndex, group ); }
		static inline void SetSpriteCategoryBits( unsigned int iSpriteIndex, unsigned int categories ) { AGKCommand265( iSpriteIndex, categories ); }
		static inline void SetSpriteCategoryBit( unsigned int iSpriteIndex, unsigned int category, int flag ) { AGKCommand266( iSpriteIndex, category, flag ); }
		static inline void SetSpriteCollideBits( unsigned int iSpriteIndex, unsigned int mask ) { AGKCommand267( iSpriteIndex, mask ); }
		static inline void SetSpriteCollideBit( unsigned int iSpriteIndex, unsigned int category, int flag ) { AGKCommand268( iSpriteIndex, category, flag ); }
		static inline void SetSpritePhysicsForce( unsigned int iSpriteIndex, float x, float y, float vx, float vy ) { AGKCommand269( iSpriteIndex, x, y, vx, vy ); }
		static inline void SetSpritePhysicsTorque( unsigned int iSpriteIndex, float torque ) { AGKCommand270( iSpriteIndex, torque ); }
		static inline void SetSpritePhysicsImpulse( unsigned int iSpriteIndex, float x, float y, float vx, float vy ) { AGKCommand271( iSpriteIndex, x, y, vx, vy ); }
		static inline void SetSpritePhysicsAngularImpulse( unsigned int iSpriteIndex, float impulse ) { AGKCommand272( iSpriteIndex, impulse ); }
		static inline float GetSpritePhysicsVelocityX( unsigned int iSpriteIndex ) { return AGKCommand273( iSpriteIndex ); }
		static inline float GetSpritePhysicsVelocityY( unsigned int iSpriteIndex ) { return AGKCommand274( iSpriteIndex ); }
		static inline float GetSpritePhysicsAngularVelocity( unsigned int iSpriteIndex ) { return AGKCommand275( iSpriteIndex ); }
		static inline float GetSpritePhysicsMass( unsigned int iSpriteIndex ) { return AGKCommand276( iSpriteIndex ); }
		static inline void DeleteJoint( unsigned int iJointIndex ) { AGKCommand277( iJointIndex ); }
		static inline int GetJointExists( unsigned int iJointIndex ) { return AGKCommand278( iJointIndex ); }
		static inline void CreateDistanceJoint( unsigned int iJointIndex, unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float x, float y, float x2, float y2, int colConnected ) { AGKCommand279( iJointIndex, iSpriteIndex1, iSpriteIndex2, x, y, x2, y2, colConnected ); }
		static inline void CreateRevoluteJoint( unsigned int iJointIndex, unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float x, float y, int colConnected ) { AGKCommand280( iJointIndex, iSpriteIndex1, iSpriteIndex2, x, y, colConnected ); }
		static inline void CreatePrismaticJoint( unsigned int iJointIndex, unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float x, float y, float vx, float vy, int colConnected ) { AGKCommand281( iJointIndex, iSpriteIndex1, iSpriteIndex2, x, y, vx, vy, colConnected ); }
		static inline void CreatePulleyJoint( unsigned int iJointIndex, unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float gnd1x, float gnd1y, float gnd2x, float gnd2y, float a1x, float a1y, float a2x, float a2y, float ratio, int colConnected ) { AGKCommand282( iJointIndex, iSpriteIndex1, iSpriteIndex2, gnd1x, gnd1y, gnd2x, gnd2y, a1x, a1y, a2x, a2y, ratio, colConnected ); }
		static inline void CreateMouseJoint( unsigned int iJointIndex, unsigned int iSpriteIndex, float x, float y, float maxForce ) { AGKCommand283( iJointIndex, iSpriteIndex, x, y, maxForce ); }
		static inline void CreateLineJoint( unsigned int iJointIndex, unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float x, float y, float vx, float vy, int colConnected ) { AGKCommand284( iJointIndex, iSpriteIndex1, iSpriteIndex2, x, y, vx, vy, colConnected ); }
		static inline void CreateWeldJoint( unsigned int iJointIndex, unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float x, float y, int colConnected ) { AGKCommand285( iJointIndex, iSpriteIndex1, iSpriteIndex2, x, y, colConnected ); }
		static inline void CreateGearJoint( unsigned int iJointIndex, unsigned int iJoint1, unsigned int iJoint2, float ratio ) { AGKCommand286( iJointIndex, iJoint1, iJoint2, ratio ); }
		static inline unsigned int CreateDistanceJoint( unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float x, float y, float x2, float y2, int colConnected ) { return AGKCommand287( iSpriteIndex1, iSpriteIndex2, x, y, x2, y2, colConnected ); }
		static inline unsigned int CreateRevoluteJoint( unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float x, float y, int colConnected ) { return AGKCommand288( iSpriteIndex1, iSpriteIndex2, x, y, colConnected ); }
		static inline unsigned int CreatePrismaticJoint( unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float x, float y, float vx, float vy, int colConnected ) { return AGKCommand289( iSpriteIndex1, iSpriteIndex2, x, y, vx, vy, colConnected ); }
		static inline void CreatePulleyJoint2( unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float ratio, int colConnected ) { AGKCommand290( iSpriteIndex1, iSpriteIndex2, ratio, colConnected ); }
		static inline unsigned int FinishPulleyJoint( float gnd1x, float gnd1y, float gnd2x, float gnd2y, float a1x, float a1y, float a2x, float a2y ) { return AGKCommand291( gnd1x, gnd1y, gnd2x, gnd2y, a1x, a1y, a2x, a2y ); }
		static inline unsigned int CreateMouseJoint( unsigned int iSpriteIndex, float x, float y, float maxForce ) { return AGKCommand292( iSpriteIndex, x, y, maxForce ); }
		static inline unsigned int CreateLineJoint( unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float x, float y, float vx, float vy, int colConnected ) { return AGKCommand293( iSpriteIndex1, iSpriteIndex2, x, y, vx, vy, colConnected ); }
		static inline unsigned int CreateWeldJoint( unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float x, float y, int colConnected ) { return AGKCommand294( iSpriteIndex1, iSpriteIndex2, x, y, colConnected ); }
		static inline unsigned int CreateGearJoint( unsigned int iJoint1, unsigned int iJoint2, float ratio ) { return AGKCommand295( iJoint1, iJoint2, ratio ); }
		static inline void SetJointLimitOn( unsigned int iJointIndex, float lowerLimit, float upperLimit ) { AGKCommand296( iJointIndex, lowerLimit, upperLimit ); }
		static inline void SetJointLimitOff( unsigned int iJointIndex ) { AGKCommand297( iJointIndex ); }
		static inline void SetJointMotorOn( unsigned int iJointIndex, float speed, float maxForce ) { AGKCommand298( iJointIndex, speed, maxForce ); }
		static inline void SetJointMotorOff( unsigned int iJointIndex ) { AGKCommand299( iJointIndex ); }
		static inline void SetJointMouseTarget( unsigned int iJointIndex, float x, float y ) { AGKCommand300( iJointIndex, x, y ); }
		static inline float GetJointReactionForceX( unsigned int iJointIndex ) { return AGKCommand301( iJointIndex ); }
		static inline float GetJointReactionForceY( unsigned int iJointIndex ) { return AGKCommand302( iJointIndex ); }
		static inline float GetJointReactionTorque( unsigned int iJointIndex ) { return AGKCommand303( iJointIndex ); }
		static inline int PhysicsRayCast( float x, float y, float x2, float y2 ) { return AGKCommand304( x, y, x2, y2 ); }
		static inline int PhysicsRayCastGroup( int group, float x, float y, float x2, float y2 ) { return AGKCommand305( group, x, y, x2, y2 ); }
		static inline int PhysicsRayCastCategory( unsigned int category, float x, float y, float x2, float y2 ) { return AGKCommand306( category, x, y, x2, y2 ); }
		static inline int SpriteRayCastSingle( unsigned int sprite, float x, float y, float x2, float y2 ) { return AGKCommand307( sprite, x, y, x2, y2 ); }
		static inline int SpriteRayCast( float x, float y, float x2, float y2 ) { return AGKCommand308( x, y, x2, y2 ); }
		static inline int SpriteRayCastGroup( int group, float x, float y, float x2, float y2 ) { return AGKCommand309( group, x, y, x2, y2 ); }
		static inline int SpriteRayCastCategory( unsigned int category, float x, float y, float x2, float y2 ) { return AGKCommand310( category, x, y, x2, y2 ); }
		static inline cSprite* GetRayCastSprite(  ) { return AGKCommand311(  ); }
		static inline unsigned int GetRayCastSpriteID(  ) { return AGKCommand312(  ); }
		static inline float GetRayCastX(  ) { return AGKCommand313(  ); }
		static inline float GetRayCastY(  ) { return AGKCommand314(  ); }
		static inline float GetRayCastNormalX(  ) { return AGKCommand315(  ); }
		static inline float GetRayCastNormalY(  ) { return AGKCommand316(  ); }
		static inline float GetRayCastFraction(  ) { return AGKCommand317(  ); }
		static inline int GetSpriteInBox( unsigned int iSprite, float x1, float y1, float x2, float y2 ) { return AGKCommand318( iSprite, x1, y1, x2, y2 ); }
		static inline int GetSpriteInCircle( unsigned int iSprite, float x1, float y1, float radius ) { return AGKCommand319( iSprite, x1, y1, radius ); }
		static inline int GetSpriteCollision( unsigned int iSprite1, unsigned int iSprite2 ) { return AGKCommand320( iSprite1, iSprite2 ); }
		static inline float GetSpriteDistance( unsigned int iSprite1, unsigned int iSprite2 ) { return AGKCommand321( iSprite1, iSprite2 ); }
		static inline float GetSpriteDistancePoint1X(  ) { return AGKCommand322(  ); }
		static inline float GetSpriteDistancePoint1Y(  ) { return AGKCommand323(  ); }
		static inline float GetSpriteDistancePoint2X(  ) { return AGKCommand324(  ); }
		static inline float GetSpriteDistancePoint2Y(  ) { return AGKCommand325(  ); }
		static inline int GetFirstContact(  ) { return AGKCommand326(  ); }
		static inline int GetNextContact(  ) { return AGKCommand327(  ); }
		static inline float GetContactWorldX(  ) { return AGKCommand328(  ); }
		static inline float GetContactWorldY(  ) { return AGKCommand329(  ); }
		static inline unsigned int GetContactSpriteID1(  ) { return AGKCommand330(  ); }
		static inline unsigned int GetContactSpriteID2(  ) { return AGKCommand331(  ); }
		static inline int GetSpriteFirstContact( unsigned int iSprite1 ) { return AGKCommand332( iSprite1 ); }
		static inline int GetSpriteNextContact(  ) { return AGKCommand333(  ); }
		static inline float GetSpriteContactWorldX(  ) { return AGKCommand334(  ); }
		static inline float GetSpriteContactWorldY(  ) { return AGKCommand335(  ); }
		static inline unsigned int GetSpriteContactSpriteID2(  ) { return AGKCommand336(  ); }
		static inline int GetPhysicsCollision( unsigned int iSprite1, unsigned int iSprite2 ) { return AGKCommand337( iSprite1, iSprite2 ); }
		static inline float GetPhysicsCollisionX(  ) { return AGKCommand338(  ); }
		static inline float GetPhysicsCollisionY(  ) { return AGKCommand339(  ); }
		static inline float GetPhysicsCollisionWorldX(  ) { return AGKCommand340(  ); }
		static inline float GetPhysicsCollisionWorldY(  ) { return AGKCommand341(  ); }
		static inline void CreateParticles( unsigned int ID, float x, float y ) { AGKCommand342( ID, x, y ); }
		static inline unsigned int CreateParticles( float x, float y ) { return AGKCommand343( x, y ); }
		static inline int GetParticlesExists( unsigned int ID ) { return AGKCommand344( ID ); }
		static inline void DeleteParticles( unsigned int ID ) { AGKCommand345( ID ); }
		static inline void SetParticlesPosition( unsigned int ID, float x, float y ) { AGKCommand346( ID, x, y ); }
		static inline void SetParticlesDepth( unsigned int ID, int depth ) { AGKCommand347( ID, depth ); }
		static inline void SetParticlesFrequency( unsigned int ID, float freq ) { AGKCommand348( ID, freq ); }
		static inline void SetParticlesStartZone( unsigned int ID, float x1, float y1, float x2, float y2 ) { AGKCommand349( ID, x1, y1, x2, y2 ); }
		static inline void SetParticlesDirection( unsigned int ID, float vx, float vy ) { AGKCommand350( ID, vx, vy ); }
		static inline void SetParticlesVelocityRange( unsigned int ID, float v1, float v2 ) { AGKCommand351( ID, v1, v2 ); }
		static inline void SetParticlesAngle( unsigned int ID, float angle ) { AGKCommand352( ID, angle ); }
		static inline void SetParticlesAngleRad( unsigned int ID, float angle ) { AGKCommand353( ID, angle ); }
		static inline void SetParticlesRotationRange( unsigned int ID, float angle1, float angle2 ) { AGKCommand354( ID, angle1, angle2 ); }
		static inline void SetParticlesRotationRangeRad( unsigned int ID, float angle1, float angle2 ) { AGKCommand355( ID, angle1, angle2 ); }
		static inline void SetParticlesFaceDirection( unsigned int ID, int mode ) { AGKCommand356( ID, mode ); }
		static inline void SetParticlesSize( unsigned int ID, float size ) { AGKCommand357( ID, size ); }
		static inline void SetParticlesLife( unsigned int ID, float time ) { AGKCommand358( ID, time ); }
		static inline void SetParticlesMax( unsigned int ID, int max ) { AGKCommand359( ID, max ); }
		static inline void ResetParticleCount( unsigned int ID ) { AGKCommand360( ID ); }
		static inline void SetParticlesImage( unsigned int ID, unsigned int imageID ) { AGKCommand361( ID, imageID ); }
		static inline void SetParticlesVisible( unsigned int ID, int visible ) { AGKCommand362( ID, visible ); }
		static inline void SetParticlesActive( unsigned int ID, int active ) { AGKCommand363( ID, active ); }
		static inline void SetParticlesTransparency( unsigned int ID, int active ) { AGKCommand364( ID, active ); }
		static inline void SetParticlesColorInterpolation( unsigned int ID, int mode ) { AGKCommand365( ID, mode ); }
		static inline float GetParticlesX( unsigned int ID ) { return AGKCommand366( ID ); }
		static inline float GetParticlesY( unsigned int ID ) { return AGKCommand367( ID ); }
		static inline int GetParticlesDepth( unsigned int ID ) { return AGKCommand368( ID ); }
		static inline int GetParticlesVisible( unsigned int ID ) { return AGKCommand369( ID ); }
		static inline int GetParticlesActive( unsigned int ID ) { return AGKCommand370( ID ); }
		static inline float GetParticlesFrequency( unsigned int ID ) { return AGKCommand371( ID ); }
		static inline float GetParticlesDirectionX( unsigned int ID ) { return AGKCommand372( ID ); }
		static inline float GetParticlesDirectionY( unsigned int ID ) { return AGKCommand373( ID ); }
		static inline float GetParticlesAngle( unsigned int ID ) { return AGKCommand374( ID ); }
		static inline float GetParticlesAngleRad( unsigned int ID ) { return AGKCommand375( ID ); }
		static inline float GetParticlesSize( unsigned int ID ) { return AGKCommand376( ID ); }
		static inline float GetParticlesLife( unsigned int ID ) { return AGKCommand377( ID ); }
		static inline int GetParticlesMaxReached( unsigned int ID ) { return AGKCommand378( ID ); }
		static inline void AddParticlesForce( unsigned int ID, float starttime, float endtime, float x, float y ) { AGKCommand379( ID, starttime, endtime, x, y ); }
		static inline void ClearParticlesForces( unsigned int ID ) { AGKCommand380( ID ); }
		static inline void AddParticlesColorKeyFrame( unsigned int ID, float time, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha ) { AGKCommand381( ID, time, red, green, blue, alpha ); }
		static inline void ClearParticlesColors( unsigned int ID ) { AGKCommand382( ID ); }
		static inline void AddParticlesScaleKeyFrame( unsigned int ID, float time, float scale ) { AGKCommand383( ID, time, scale ); }
		static inline void ClearParticlesScales( unsigned int ID ) { AGKCommand384( ID ); }
		static inline void FixParticlesToScreen( unsigned int ID, int mode ) { AGKCommand385( ID, mode ); }
		static inline void UpdateParticles( unsigned int ID, float time ) { AGKCommand386( ID, time ); }
		static inline void OffsetParticles( unsigned int ID, float x, float y ) { AGKCommand387( ID, x, y ); }
		static inline void CreateText( unsigned int iTextIndex, const char * string ) { AGKCommand388( iTextIndex, string ); }
		static inline unsigned int CreateText( const char * string ) { return AGKCommand389( string ); }
		static inline unsigned int GetTextExists( unsigned int iTextIndex ) { return AGKCommand390( iTextIndex ); }
		static inline void DeleteText( unsigned int iTextIndex ) { AGKCommand391( iTextIndex ); }
		static inline void DeleteAllText(  ) { AGKCommand392(  ); }
		static inline void SetTextString( unsigned int iTextIndex, const char * string ) { AGKCommand393( iTextIndex, string ); }
		static inline void SetTextPosition( unsigned int iTextIndex, float fX, float fY ) { AGKCommand394( iTextIndex, fX, fY ); }
		static inline void SetTextX( unsigned int iTextIndex, float fX ) { AGKCommand395( iTextIndex, fX ); }
		static inline void SetTextY( unsigned int iTextIndex, float fY ) { AGKCommand396( iTextIndex, fY ); }
		static inline void SetTextAngle( unsigned int iTextIndex, float fAngle ) { AGKCommand397( iTextIndex, fAngle ); }
		static inline void SetTextAngleRad( unsigned int iTextIndex, float fAngleRad ) { AGKCommand398( iTextIndex, fAngleRad ); }
		static inline void SetTextAlignment( unsigned int iTextIndex, int iMode ) { AGKCommand399( iTextIndex, iMode ); }
		static inline void SetTextSize( unsigned int iTextIndex, float fSize ) { AGKCommand400( iTextIndex, fSize ); }
		static inline void SetTextSpacing( unsigned int iTextIndex, float fSpacing ) { AGKCommand401( iTextIndex, fSpacing ); }
		static inline void SetTextLineSpacing( unsigned int iTextIndex, float fSpacing ) { AGKCommand402( iTextIndex, fSpacing ); }
		static inline void SetTextDepth( unsigned int iTextIndex, int iDepth ) { AGKCommand403( iTextIndex, iDepth ); }
		static inline void SetTextVisible( unsigned int iTextIndex, int bVisible ) { AGKCommand404( iTextIndex, bVisible ); }
		static inline void SetTextColor( unsigned int iTextIndex, unsigned int iRed, unsigned int iGreen, unsigned int iBlue, unsigned int iAlpha ) { AGKCommand405( iTextIndex, iRed, iGreen, iBlue, iAlpha ); }
		static inline void SetTextColorRed( unsigned int iTextIndex, unsigned int iRed ) { AGKCommand406( iTextIndex, iRed ); }
		static inline void SetTextColorGreen( unsigned int iTextIndex, unsigned int iGreen ) { AGKCommand407( iTextIndex, iGreen ); }
		static inline void SetTextColorBlue( unsigned int iTextIndex, unsigned int iBlue ) { AGKCommand408( iTextIndex, iBlue ); }
		static inline void SetTextColorAlpha( unsigned int iTextIndex, unsigned int iAlpha ) { AGKCommand409( iTextIndex, iAlpha ); }
		static inline unsigned int GetTextColorRed( unsigned int iTextIndex ) { return AGKCommand410( iTextIndex ); }
		static inline unsigned int GetTextColorGreen( unsigned int iTextIndex ) { return AGKCommand411( iTextIndex ); }
		static inline unsigned int GetTextColorBlue( unsigned int iTextIndex ) { return AGKCommand412( iTextIndex ); }
		static inline unsigned int GetTextColorAlpha( unsigned int iTextIndex ) { return AGKCommand413( iTextIndex ); }
		static inline void SetTextCharPosition( unsigned int iTextIndex, unsigned int iCharIndex, float x, float y ) { AGKCommand414( iTextIndex, iCharIndex, x, y ); }
		static inline void SetTextCharX( unsigned int iTextIndex, unsigned int iCharIndex, float x ) { AGKCommand415( iTextIndex, iCharIndex, x ); }
		static inline void SetTextCharY( unsigned int iTextIndex, unsigned int iCharIndex, float y ) { AGKCommand416( iTextIndex, iCharIndex, y ); }
		static inline void SetTextCharAngle( unsigned int iTextIndex, unsigned int iCharIndex, float angle ) { AGKCommand417( iTextIndex, iCharIndex, angle ); }
		static inline void SetTextCharAngleRad( unsigned int iTextIndex, unsigned int iCharIndex, float angle ) { AGKCommand418( iTextIndex, iCharIndex, angle ); }
		static inline void SetTextCharColor( unsigned int iTextIndex, unsigned int iCharIndex, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha ) { AGKCommand419( iTextIndex, iCharIndex, red, green, blue, alpha ); }
		static inline void SetTextCharColorRed( unsigned int iTextIndex, unsigned int iCharIndex, unsigned int red ) { AGKCommand420( iTextIndex, iCharIndex, red ); }
		static inline void SetTextCharColorGreen( unsigned int iTextIndex, unsigned int iCharIndex, unsigned int green ) { AGKCommand421( iTextIndex, iCharIndex, green ); }
		static inline void SetTextCharColorBlue( unsigned int iTextIndex, unsigned int iCharIndex, unsigned int blue ) { AGKCommand422( iTextIndex, iCharIndex, blue ); }
		static inline void SetTextCharColorAlpha( unsigned int iTextIndex, unsigned int iCharIndex, unsigned int alpha ) { AGKCommand423( iTextIndex, iCharIndex, alpha ); }
		static inline void SetTextCharBold( unsigned int iTextIndex, unsigned int iCharIndex, unsigned int bold ) { AGKCommand424( iTextIndex, iCharIndex, bold ); }
		static inline float GetTextCharX( unsigned int iTextIndex, unsigned int iCharIndex ) { return AGKCommand425( iTextIndex, iCharIndex ); }
		static inline float GetTextCharY( unsigned int iTextIndex, unsigned int iCharIndex ) { return AGKCommand426( iTextIndex, iCharIndex ); }
		static inline float GetTextCharAngle( unsigned int iTextIndex, unsigned int iCharIndex ) { return AGKCommand427( iTextIndex, iCharIndex ); }
		static inline float GetTextCharAngleRad( unsigned int iTextIndex, unsigned int iCharIndex ) { return AGKCommand428( iTextIndex, iCharIndex ); }
		static inline unsigned int GetTextCharColorRed( unsigned int iTextIndex, unsigned int iCharIndex ) { return AGKCommand429( iTextIndex, iCharIndex ); }
		static inline unsigned int GetTextCharColorGreen( unsigned int iTextIndex, unsigned int iCharIndex ) { return AGKCommand430( iTextIndex, iCharIndex ); }
		static inline unsigned int GetTextCharColorBlue( unsigned int iTextIndex, unsigned int iCharIndex ) { return AGKCommand431( iTextIndex, iCharIndex ); }
		static inline unsigned int GetTextCharColorAlpha( unsigned int iTextIndex, unsigned int iCharIndex ) { return AGKCommand432( iTextIndex, iCharIndex ); }
		static inline float GetTextX( unsigned int iTextIndex ) { return AGKCommand433( iTextIndex ); }
		static inline float GetTextY( unsigned int iTextIndex ) { return AGKCommand434( iTextIndex ); }
		static inline unsigned int GetTextLength( unsigned int iTextIndex ) { return AGKCommand435( iTextIndex ); }
		static inline float GetTextTotalWidth( unsigned int iTextIndex ) { return AGKCommand436( iTextIndex ); }
		static inline float GetTextTotalHeight( unsigned int iTextIndex ) { return AGKCommand437( iTextIndex ); }
		static inline int GetTextHitTest( unsigned int iTextIndex, float x, float y ) { return AGKCommand438( iTextIndex, x, y ); }
		static inline void SetTextFontImage( unsigned int iTextIndex, unsigned int iImageID ) { AGKCommand439( iTextIndex, iImageID ); }
		static inline void SetTextExtendedFontImage( unsigned int iTextIndex, unsigned int iImageID ) { AGKCommand440( iTextIndex, iImageID ); }
		static inline void SetTextFont( unsigned int iTextIndex, unsigned int iFontID ) { AGKCommand441( iTextIndex, iFontID ); }
		static inline void SetTextDefaultFontImage( unsigned int iImageID ) { AGKCommand442( iImageID ); }
		static inline void SetTextDefaultExtendedFontImage( unsigned int iImageID ) { AGKCommand443( iImageID ); }
		static inline void SetTextDefaultMinFilter( unsigned int mode ) { AGKCommand444( mode ); }
		static inline void SetTextDefaultMagFilter( unsigned int mode ) { AGKCommand445( mode ); }
		static inline void FixTextToScreen( unsigned int iTextIndex, int mode ) { AGKCommand446( iTextIndex, mode ); }
		static inline void SetTextMaxWidth( unsigned int iTextIndex, float width ) { AGKCommand447( iTextIndex, width ); }
		static inline void SetTextScissor( unsigned int iTextIndex, float x, float y, float x2, float y2 ) { AGKCommand448( iTextIndex, x, y, x2, y2 ); }
		static inline void SetTextTransparency( unsigned int iTextIndex, int mode ) { AGKCommand449( iTextIndex, mode ); }
		static inline void SetTextBold( unsigned int iTextIndex, unsigned int bold ) { AGKCommand450( iTextIndex, bold ); }
		static inline int GetTextVisible( unsigned int iTextIndex ) { return AGKCommand451( iTextIndex ); }
		static inline int GetTextDepth( unsigned int iTextIndex ) { return AGKCommand452( iTextIndex ); }
		static inline float GetTextSize( unsigned int iTextIndex ) { return AGKCommand453( iTextIndex ); }
		static inline float GetTextSpacing( unsigned int iTextIndex ) { return AGKCommand454( iTextIndex ); }
		static inline float GetTextLineSpacing( unsigned int iTextIndex ) { return AGKCommand455( iTextIndex ); }
		static inline char* GetTextString( unsigned int iTextIndex ) { return AGKCommand456( iTextIndex ); }
		static inline unsigned int LoadFont( const char * szFontFile ) { return AGKCommand457( szFontFile ); }
		static inline void LoadFont( unsigned int iFontID, const char * szFontFile ) { AGKCommand458( iFontID, szFontFile ); }
		static inline int GetFontExists( unsigned int iFontID ) { return AGKCommand459( iFontID ); }
		static inline int GetSystemFontExists( const char * szFontFile ) { return AGKCommand460( szFontFile ); }
		static inline void DeleteFont( unsigned int iFontID ) { AGKCommand461( iFontID ); }
		static inline void Print( const char * szString ) { AGKCommand462( szString ); }
		static inline void Print( int i ) { AGKCommand463( i ); }
		static inline void Print( float f ) { AGKCommand464( f ); }
		static inline void PrintC( const char * szString ) { AGKCommand465( szString ); }
		static inline void PrintC( int i ) { AGKCommand466( i ); }
		static inline void PrintC( float f ) { AGKCommand467( f ); }
		static inline void SetPrintSize( float fSize ) { AGKCommand468( fSize ); }
		static inline void SetPrintSpacing( float fSpacing ) { AGKCommand469( fSpacing ); }
		static inline void SetPrintColor( unsigned int iRed, unsigned int iGreen, unsigned int iBlue ) { AGKCommand470( iRed, iGreen, iBlue ); }
		static inline void SetPrintColor( unsigned int iRed, unsigned int iGreen, unsigned int iBlue, unsigned int iAlpha ) { AGKCommand471( iRed, iGreen, iBlue, iAlpha ); }
		static inline void SetPrintFont( int fontID ) { AGKCommand472( fontID ); }
		static inline void CreateSkeleton2D( unsigned int iSkeleton ) { AGKCommand473( iSkeleton ); }
		static inline unsigned int CreateSkeleton2D(  ) { return AGKCommand474(  ); }
		static inline int GetSkeleton2DExists( unsigned int iSkeleton ) { return AGKCommand475( iSkeleton ); }
		static inline void DeleteSkeleton2D( unsigned int iSkeleton ) { AGKCommand476( iSkeleton ); }
		static inline void LoadSkeleton2DFromSpineFile( unsigned int iSkeleton, const char* filename, float scale, unsigned int atlasImage, int loadAnim ) { AGKCommand477( iSkeleton, filename, scale, atlasImage, loadAnim ); }
		static inline unsigned int LoadSkeleton2DFromSpineFile( const char* filename, float scale, unsigned int atlasImage, int loadAnim ) { return AGKCommand478( filename, scale, atlasImage, loadAnim ); }
		static inline void LoadSkeleton2DFromSpriterFile( unsigned int iSkeleton, const char* filename, float scale, unsigned int atlasImage ) { AGKCommand479( iSkeleton, filename, scale, atlasImage ); }
		static inline unsigned int LoadSkeleton2DFromSpriterFile( const char* filename, float scale, unsigned int atlasImage ) { return AGKCommand480( filename, scale, atlasImage ); }
		static inline void SetSkeleton2DPosition( unsigned int iSkeleton, float x, float y ) { AGKCommand481( iSkeleton, x, y ); }
		static inline void SetSkeleton2DRotation( unsigned int iSkeleton, float r ) { AGKCommand482( iSkeleton, r ); }
		static inline void SetSkeleton2DFlip( unsigned int iSkeleton, int flipH, int flipV ) { AGKCommand483( iSkeleton, flipH, flipV ); }
		static inline void SetSkeleton2DDepth( unsigned int iSkeleton, unsigned int depth ) { AGKCommand484( iSkeleton, depth ); }
		static inline void FixSkeleton2DToScreen( unsigned int iSkeleton, int mode ) { AGKCommand485( iSkeleton, mode ); }
		static inline void SetSkeleton2DVisible( unsigned int iSkeleton, int mode ) { AGKCommand486( iSkeleton, mode ); }
		static inline float GetSkeleton2DX( unsigned int iSkeleton ) { return AGKCommand487( iSkeleton ); }
		static inline float GetSkeleton2DY( unsigned int iSkeleton ) { return AGKCommand488( iSkeleton ); }
		static inline float GetSkeleton2DAngle( unsigned int iSkeleton ) { return AGKCommand489( iSkeleton ); }
		static inline int GetSkeleton2DBone( unsigned int iSkeleton, const char* name ) { return AGKCommand490( iSkeleton, name ); }
		static inline int GetSkeleton2DBoneParent( unsigned int iSkeleton, int bone ) { return AGKCommand491( iSkeleton, bone ); }
		static inline float GetSkeleton2DBoneX( unsigned int iSkeleton, int bone ) { return AGKCommand492( iSkeleton, bone ); }
		static inline float GetSkeleton2DBoneY( unsigned int iSkeleton, int bone ) { return AGKCommand493( iSkeleton, bone ); }
		static inline float GetSkeleton2DBoneAngle( unsigned int iSkeleton, int bone ) { return AGKCommand494( iSkeleton, bone ); }
		static inline float GetSkeleton2DBoneCurrX( unsigned int iSkeleton, int bone ) { return AGKCommand495( iSkeleton, bone ); }
		static inline float GetSkeleton2DBoneCurrY( unsigned int iSkeleton, int bone ) { return AGKCommand496( iSkeleton, bone ); }
		static inline float GetSkeleton2DBoneCurrAngle( unsigned int iSkeleton, int bone ) { return AGKCommand497( iSkeleton, bone ); }
		static inline void SetSkeleton2DBonePosition( unsigned int iSkeleton, int bone, float x, float y ) { AGKCommand498( iSkeleton, bone, x, y ); }
		static inline void SetSkeleton2DBoneAngle( unsigned int iSkeleton, int bone, float r ) { AGKCommand499( iSkeleton, bone, r ); }
		static inline void SetSkeleton2DBoneScale( unsigned int iSkeleton, int bone, float sx, float sy ) { AGKCommand500( iSkeleton, bone, sx, sy ); }
		static inline void SetSkeleton2DBoneMode( unsigned int iSkeleton, int bone, int mode ) { AGKCommand501( iSkeleton, bone, mode ); }
		static inline void PlaySkeleton2DAnimation( unsigned int iSkeleton, const char* anim, float starttime, int loop, float tweentime ) { AGKCommand502( iSkeleton, anim, starttime, loop, tweentime ); }
		static inline void SetSkeleton2DAnimationFrame( unsigned int iSkeleton, const char* anim, float time, float tweentime ) { AGKCommand503( iSkeleton, anim, time, tweentime ); }
		static inline void SetSkeleton2DAnimationSpeed( unsigned int iSkeleton, float speed ) { AGKCommand504( iSkeleton, speed ); }
		static inline float GetSkeleton2DCurrentTime( unsigned int iSkeleton ) { return AGKCommand505( iSkeleton ); }
		static inline void StopSkeleton2DAnimation( unsigned int iSkeleton ) { AGKCommand506( iSkeleton ); }
		static inline int GetSkeleton2DIsAnimating( unsigned int iSkeleton ) { return AGKCommand507( iSkeleton ); }
		static inline int GetSkeleton2DIsTweening( unsigned int iSkeleton ) { return AGKCommand508( iSkeleton ); }
		static inline float GetSkeleton2DAnimationTime( unsigned int iSkeleton, const char* anim ) { return AGKCommand509( iSkeleton, anim ); }
		static inline int TweenLinear(  ) { return AGKCommand510(  ); }
		static inline int TweenSmooth1(  ) { return AGKCommand511(  ); }
		static inline int TweenSmooth2(  ) { return AGKCommand512(  ); }
		static inline int TweenEaseIn1(  ) { return AGKCommand513(  ); }
		static inline int TweenEaseIn2(  ) { return AGKCommand514(  ); }
		static inline int TweenEaseOut1(  ) { return AGKCommand515(  ); }
		static inline int TweenEaseOut2(  ) { return AGKCommand516(  ); }
		static inline int TweenBounce(  ) { return AGKCommand517(  ); }
		static inline int TweenOvershoot(  ) { return AGKCommand518(  ); }
		static inline void DeleteTween( unsigned int tweenID ) { AGKCommand519( tweenID ); }
		static inline int GetTweenExists( unsigned int tweenID ) { return AGKCommand520( tweenID ); }
		static inline void SetTweenDuration( unsigned int tweenID, float duration ) { AGKCommand521( tweenID, duration ); }
		static inline void CreateTweenCustom( unsigned int tweenID, float duration ) { AGKCommand522( tweenID, duration ); }
		static inline unsigned int CreateTweenCustom( float duration ) { return AGKCommand523( duration ); }
		static inline int GetTweenCustomExists( unsigned int tweenID ) { return AGKCommand524( tweenID ); }
		static inline void SetTweenCustomFloat1( unsigned int tweenID, float begin, float end, int interpolation ) { AGKCommand525( tweenID, begin, end, interpolation ); }
		static inline void SetTweenCustomFloat2( unsigned int tweenID, float begin, float end, int interpolation ) { AGKCommand526( tweenID, begin, end, interpolation ); }
		static inline void SetTweenCustomFloat3( unsigned int tweenID, float begin, float end, int interpolation ) { AGKCommand527( tweenID, begin, end, interpolation ); }
		static inline void SetTweenCustomFloat4( unsigned int tweenID, float begin, float end, int interpolation ) { AGKCommand528( tweenID, begin, end, interpolation ); }
		static inline void SetTweenCustomInteger1( unsigned int tweenID, int begin, int end, int interpolation ) { AGKCommand529( tweenID, begin, end, interpolation ); }
		static inline void SetTweenCustomInteger2( unsigned int tweenID, int begin, int end, int interpolation ) { AGKCommand530( tweenID, begin, end, interpolation ); }
		static inline void SetTweenCustomInteger3( unsigned int tweenID, int begin, int end, int interpolation ) { AGKCommand531( tweenID, begin, end, interpolation ); }
		static inline void SetTweenCustomInteger4( unsigned int tweenID, int begin, int end, int interpolation ) { AGKCommand532( tweenID, begin, end, interpolation ); }
		static inline float GetTweenCustomFloat1( unsigned int tweenID ) { return AGKCommand533( tweenID ); }
		static inline float GetTweenCustomFloat2( unsigned int tweenID ) { return AGKCommand534( tweenID ); }
		static inline float GetTweenCustomFloat3( unsigned int tweenID ) { return AGKCommand535( tweenID ); }
		static inline float GetTweenCustomFloat4( unsigned int tweenID ) { return AGKCommand536( tweenID ); }
		static inline int GetTweenCustomInteger1( unsigned int tweenID ) { return AGKCommand537( tweenID ); }
		static inline int GetTweenCustomInteger2( unsigned int tweenID ) { return AGKCommand538( tweenID ); }
		static inline int GetTweenCustomInteger3( unsigned int tweenID ) { return AGKCommand539( tweenID ); }
		static inline int GetTweenCustomInteger4( unsigned int tweenID ) { return AGKCommand540( tweenID ); }
		static inline void PlayTweenCustom( unsigned int tweenID, float delay ) { AGKCommand541( tweenID, delay ); }
		static inline void StopTweenCustom( unsigned int tweenID ) { AGKCommand542( tweenID ); }
		static inline int GetTweenCustomPlaying( unsigned int tweenID ) { return AGKCommand543( tweenID ); }
		static inline void CreateTweenSprite( unsigned int tweenID, float duration ) { AGKCommand544( tweenID, duration ); }
		static inline unsigned int CreateTweenSprite( float duration ) { return AGKCommand545( duration ); }
		static inline int GetTweenSpriteExists( unsigned int tweenID ) { return AGKCommand546( tweenID ); }
		static inline void SetTweenSpriteX( unsigned int tweenID, float beginX, float endX, int interpolation ) { AGKCommand547( tweenID, beginX, endX, interpolation ); }
		static inline void SetTweenSpriteY( unsigned int tweenID, float beginY, float endY, int interpolation ) { AGKCommand548( tweenID, beginY, endY, interpolation ); }
		static inline void SetTweenSpriteXByOffset( unsigned int tweenID, float beginX, float endX, int interpolation ) { AGKCommand549( tweenID, beginX, endX, interpolation ); }
		static inline void SetTweenSpriteYByOffset( unsigned int tweenID, float beginY, float endY, int interpolation ) { AGKCommand550( tweenID, beginY, endY, interpolation ); }
		static inline void SetTweenSpriteAngle( unsigned int tweenID, float beginA, float endA, int interpolation ) { AGKCommand551( tweenID, beginA, endA, interpolation ); }
		static inline void SetTweenSpriteSizeX( unsigned int tweenID, float beginSX, float endSX, int interpolation ) { AGKCommand552( tweenID, beginSX, endSX, interpolation ); }
		static inline void SetTweenSpriteSizeY( unsigned int tweenID, float beginSY, float endSY, int interpolation ) { AGKCommand553( tweenID, beginSY, endSY, interpolation ); }
		static inline void SetTweenSpriteRed( unsigned int tweenID, int beginR, int endR, int interpolation ) { AGKCommand554( tweenID, beginR, endR, interpolation ); }
		static inline void SetTweenSpriteGreen( unsigned int tweenID, int beginG, int endG, int interpolation ) { AGKCommand555( tweenID, beginG, endG, interpolation ); }
		static inline void SetTweenSpriteBlue( unsigned int tweenID, int beginB, int endB, int interpolation ) { AGKCommand556( tweenID, beginB, endB, interpolation ); }
		static inline void SetTweenSpriteAlpha( unsigned int tweenID, int beginA, int endA, int interpolation ) { AGKCommand557( tweenID, beginA, endA, interpolation ); }
		static inline void PlayTweenSprite( unsigned int tweenID, unsigned int spriteID, float delay ) { AGKCommand558( tweenID, spriteID, delay ); }
		static inline void StopTweenSprite( unsigned int tweenID, unsigned int spriteID ) { AGKCommand559( tweenID, spriteID ); }
		static inline int GetTweenSpritePlaying( unsigned int tweenID, unsigned int spriteID ) { return AGKCommand560( tweenID, spriteID ); }
		static inline void CreateTweenText( unsigned int tweenID, float duration ) { AGKCommand561( tweenID, duration ); }
		static inline unsigned int CreateTweenText( float duration ) { return AGKCommand562( duration ); }
		static inline int GetTweenTextExists( unsigned int tweenID ) { return AGKCommand563( tweenID ); }
		static inline void SetTweenTextX( unsigned int tweenID, float beginX, float endX, int interpolation ) { AGKCommand564( tweenID, beginX, endX, interpolation ); }
		static inline void SetTweenTextY( unsigned int tweenID, float beginY, float endY, int interpolation ) { AGKCommand565( tweenID, beginY, endY, interpolation ); }
		static inline void SetTweenTextAngle( unsigned int tweenID, float beginA, float endA, int interpolation ) { AGKCommand566( tweenID, beginA, endA, interpolation ); }
		static inline void SetTweenTextSize( unsigned int tweenID, float beginS, float endS, int interpolation ) { AGKCommand567( tweenID, beginS, endS, interpolation ); }
		static inline void SetTweenTextSpacing( unsigned int tweenID, float beginSP, float endSP, int interpolation ) { AGKCommand568( tweenID, beginSP, endSP, interpolation ); }
		static inline void SetTweenTextLineSpacing( unsigned int tweenID, float beginLSP, float endLSP, int interpolation ) { AGKCommand569( tweenID, beginLSP, endLSP, interpolation ); }
		static inline void SetTweenTextRed( unsigned int tweenID, int beginR, int endR, int interpolation ) { AGKCommand570( tweenID, beginR, endR, interpolation ); }
		static inline void SetTweenTextGreen( unsigned int tweenID, int beginG, int endG, int interpolation ) { AGKCommand571( tweenID, beginG, endG, interpolation ); }
		static inline void SetTweenTextBlue( unsigned int tweenID, int beginB, int endB, int interpolation ) { AGKCommand572( tweenID, beginB, endB, interpolation ); }
		static inline void SetTweenTextAlpha( unsigned int tweenID, int beginA, int endA, int interpolation ) { AGKCommand573( tweenID, beginA, endA, interpolation ); }
		static inline void PlayTweenText( unsigned int tweenID, unsigned int textID, float delay ) { AGKCommand574( tweenID, textID, delay ); }
		static inline void StopTweenText( unsigned int tweenID, unsigned int textID ) { AGKCommand575( tweenID, textID ); }
		static inline int GetTweenTextPlaying( unsigned int tweenID, unsigned int textID ) { return AGKCommand576( tweenID, textID ); }
		static inline void CreateTweenChar( unsigned int tweenID, float duration ) { AGKCommand577( tweenID, duration ); }
		static inline unsigned int CreateTweenChar( float duration ) { return AGKCommand578( duration ); }
		static inline int GetTweenCharExists( unsigned int tweenID ) { return AGKCommand579( tweenID ); }
		static inline void SetTweenCharX( unsigned int tweenID, float beginX, float endX, int interpolation ) { AGKCommand580( tweenID, beginX, endX, interpolation ); }
		static inline void SetTweenCharY( unsigned int tweenID, float beginY, float endY, int interpolation ) { AGKCommand581( tweenID, beginY, endY, interpolation ); }
		static inline void SetTweenCharAngle( unsigned int tweenID, float beginA, float endA, int interpolation ) { AGKCommand582( tweenID, beginA, endA, interpolation ); }
		static inline void SetTweenCharRed( unsigned int tweenID, int beginR, int endR, int interpolation ) { AGKCommand583( tweenID, beginR, endR, interpolation ); }
		static inline void SetTweenCharGreen( unsigned int tweenID, int beginG, int endG, int interpolation ) { AGKCommand584( tweenID, beginG, endG, interpolation ); }
		static inline void SetTweenCharBlue( unsigned int tweenID, int beginB, int endB, int interpolation ) { AGKCommand585( tweenID, beginB, endB, interpolation ); }
		static inline void SetTweenCharAlpha( unsigned int tweenID, int beginA, int endA, int interpolation ) { AGKCommand586( tweenID, beginA, endA, interpolation ); }
		static inline void PlayTweenChar( unsigned int tweenID, unsigned int textID, unsigned int charID, float delay ) { AGKCommand587( tweenID, textID, charID, delay ); }
		static inline void StopTweenChar( unsigned int tweenID, unsigned int textID, unsigned int charID ) { AGKCommand588( tweenID, textID, charID ); }
		static inline int GetTweenCharPlaying( unsigned int tweenID, unsigned int textID, unsigned int charID ) { return AGKCommand589( tweenID, textID, charID ); }
		static inline void CreateTweenObject( unsigned int tweenID, float duration ) { AGKCommand590( tweenID, duration ); }
		static inline unsigned int CreateTweenObject( float duration ) { return AGKCommand591( duration ); }
		static inline int GetTweenObjectExists( unsigned int tweenID ) { return AGKCommand592( tweenID ); }
		static inline void SetTweenObjectX( unsigned int tweenID, float beginX, float endX, int interpolation ) { AGKCommand593( tweenID, beginX, endX, interpolation ); }
		static inline void SetTweenObjectY( unsigned int tweenID, float beginY, float endY, int interpolation ) { AGKCommand594( tweenID, beginY, endY, interpolation ); }
		static inline void SetTweenObjectZ( unsigned int tweenID, float beginZ, float endZ, int interpolation ) { AGKCommand595( tweenID, beginZ, endZ, interpolation ); }
		static inline void SetTweenObjectAngleX( unsigned int tweenID, float beginAX, float endAX, int interpolation ) { AGKCommand596( tweenID, beginAX, endAX, interpolation ); }
		static inline void SetTweenObjectAngleY( unsigned int tweenID, float beginAY, float endAY, int interpolation ) { AGKCommand597( tweenID, beginAY, endAY, interpolation ); }
		static inline void SetTweenObjectAngleZ( unsigned int tweenID, float beginAZ, float endAZ, int interpolation ) { AGKCommand598( tweenID, beginAZ, endAZ, interpolation ); }
		static inline void SetTweenObjectScaleX( unsigned int tweenID, float beginSX, float endSX, int interpolation ) { AGKCommand599( tweenID, beginSX, endSX, interpolation ); }
		static inline void SetTweenObjectScaleY( unsigned int tweenID, float beginSY, float endSY, int interpolation ) { AGKCommand600( tweenID, beginSY, endSY, interpolation ); }
		static inline void SetTweenObjectScaleZ( unsigned int tweenID, float beginSZ, float endSZ, int interpolation ) { AGKCommand601( tweenID, beginSZ, endSZ, interpolation ); }
		static inline void SetTweenObjectRed( unsigned int tweenID, int beginR, int endR, int interpolation ) { AGKCommand602( tweenID, beginR, endR, interpolation ); }
		static inline void SetTweenObjectGreen( unsigned int tweenID, int beginG, int endG, int interpolation ) { AGKCommand603( tweenID, beginG, endG, interpolation ); }
		static inline void SetTweenObjectBlue( unsigned int tweenID, int beginB, int endB, int interpolation ) { AGKCommand604( tweenID, beginB, endB, interpolation ); }
		static inline void SetTweenObjectAlpha( unsigned int tweenID, int beginA, int endA, int interpolation ) { AGKCommand605( tweenID, beginA, endA, interpolation ); }
		static inline void PlayTweenObject( unsigned int tweenID, unsigned int objectID, float delay ) { AGKCommand606( tweenID, objectID, delay ); }
		static inline void StopTweenObject( unsigned int tweenID, unsigned int objectID ) { AGKCommand607( tweenID, objectID ); }
		static inline int GetTweenObjectPlaying( unsigned int tweenID, unsigned int objectID ) { return AGKCommand608( tweenID, objectID ); }
		static inline void CreateTweenCamera( unsigned int tweenID, float duration ) { AGKCommand609( tweenID, duration ); }
		static inline unsigned int CreateTweenCamera( float duration ) { return AGKCommand610( duration ); }
		static inline int GetTweenCameraExists( unsigned int tweenID ) { return AGKCommand611( tweenID ); }
		static inline void SetTweenCameraX( unsigned int tweenID, float beginX, float endX, int interpolation ) { AGKCommand612( tweenID, beginX, endX, interpolation ); }
		static inline void SetTweenCameraY( unsigned int tweenID, float beginY, float endY, int interpolation ) { AGKCommand613( tweenID, beginY, endY, interpolation ); }
		static inline void SetTweenCameraZ( unsigned int tweenID, float beginZ, float endZ, int interpolation ) { AGKCommand614( tweenID, beginZ, endZ, interpolation ); }
		static inline void SetTweenCameraAngleX( unsigned int tweenID, float beginAX, float endAX, int interpolation ) { AGKCommand615( tweenID, beginAX, endAX, interpolation ); }
		static inline void SetTweenCameraAngleY( unsigned int tweenID, float beginAY, float endAY, int interpolation ) { AGKCommand616( tweenID, beginAY, endAY, interpolation ); }
		static inline void SetTweenCameraAngleZ( unsigned int tweenID, float beginAZ, float endAZ, int interpolation ) { AGKCommand617( tweenID, beginAZ, endAZ, interpolation ); }
		static inline void SetTweenCameraFOV( unsigned int tweenID, float beginF, float endF, int interpolation ) { AGKCommand618( tweenID, beginF, endF, interpolation ); }
		static inline void PlayTweenCamera( unsigned int tweenID, unsigned int cameraID, float delay ) { AGKCommand619( tweenID, cameraID, delay ); }
		static inline void StopTweenCamera( unsigned int tweenID, unsigned int cameraID ) { AGKCommand620( tweenID, cameraID ); }
		static inline int GetTweenCameraPlaying( unsigned int tweenID, unsigned int cameraID ) { return AGKCommand621( tweenID, cameraID ); }
		static inline void CreateTweenChain( unsigned int chainID ) { AGKCommand622( chainID ); }
		static inline unsigned int CreateTweenChain(  ) { return AGKCommand623(  ); }
		static inline void DeleteTweenChain( unsigned int chainID ) { AGKCommand624( chainID ); }
		static inline void ClearTweenChain( unsigned int chainID ) { AGKCommand625( chainID ); }
		static inline void AddTweenChainCustom( unsigned int chainID, unsigned int tweenID, float delay ) { AGKCommand626( chainID, tweenID, delay ); }
		static inline void AddTweenChainSprite( unsigned int chainID, unsigned int tweenID, unsigned int spriteID, float delay ) { AGKCommand627( chainID, tweenID, spriteID, delay ); }
		static inline void AddTweenChainText( unsigned int chainID, unsigned int tweenID, unsigned int textID, float delay ) { AGKCommand628( chainID, tweenID, textID, delay ); }
		static inline void AddTweenChainChar( unsigned int chainID, unsigned int tweenID, unsigned int textID, unsigned int charID, float delay ) { AGKCommand629( chainID, tweenID, textID, charID, delay ); }
		static inline void AddTweenChainObject( unsigned int chainID, unsigned int tweenID, unsigned int objectID, float delay ) { AGKCommand630( chainID, tweenID, objectID, delay ); }
		static inline void AddTweenChainCamera( unsigned int chainID, unsigned int tweenID, unsigned int cameraID, float delay ) { AGKCommand631( chainID, tweenID, cameraID, delay ); }
		static inline void PlayTweenChain( unsigned int chainID ) { AGKCommand632( chainID ); }
		static inline void PauseTweenChain( unsigned int chainID ) { AGKCommand633( chainID ); }
		static inline void StopTweenChain( unsigned int chainID ) { AGKCommand634( chainID ); }
		static inline int GetTweenChainPlaying( unsigned int chainID ) { return AGKCommand635( chainID ); }
		static inline void SetTweenChainTime( unsigned int chainID, float time ) { AGKCommand636( chainID, time ); }
		static inline float GetTweenChainEndTime( unsigned int chainID ) { return AGKCommand637( chainID ); }
		static inline void UpdateAllTweens( float fTime ) { AGKCommand638( fTime ); }
		static inline void UpdateTweenCustom( unsigned int tweenID, float fTime ) { AGKCommand639( tweenID, fTime ); }
		static inline void UpdateTweenSprite( unsigned int tweenID, unsigned int spriteID, float fTime ) { AGKCommand640( tweenID, spriteID, fTime ); }
		static inline void UpdateTweenText( unsigned int tweenID, unsigned int textID, float fTime ) { AGKCommand641( tweenID, textID, fTime ); }
		static inline void UpdateTweenChar( unsigned int tweenID, unsigned int textID, unsigned int charID, float fTime ) { AGKCommand642( tweenID, textID, charID, fTime ); }
		static inline void UpdateTweenObject( unsigned int tweenID, unsigned int objectID, float fTime ) { AGKCommand643( tweenID, objectID, fTime ); }
		static inline void UpdateTweenCamera( unsigned int tweenID, unsigned int cameraID, float fTime ) { AGKCommand644( tweenID, cameraID, fTime ); }
		static inline void UpdateTweenChain( unsigned int chainID, float fTime ) { AGKCommand645( chainID, fTime ); }
		static inline unsigned int GetRawTouchCount( int bIncludeUnknown ) { return AGKCommand646( bIncludeUnknown ); }
		static inline unsigned int GetRawFirstTouchEvent( int bIncludeUnknown ) { return AGKCommand647( bIncludeUnknown ); }
		static inline unsigned int GetRawNextTouchEvent(  ) { return AGKCommand648(  ); }
		static inline int GetRawTouchType( unsigned int iIndex ) { return AGKCommand649( iIndex ); }
		static inline float GetRawTouchStartX( unsigned int iIndex ) { return AGKCommand650( iIndex ); }
		static inline float GetRawTouchStartY( unsigned int iIndex ) { return AGKCommand651( iIndex ); }
		static inline float GetRawTouchCurrentX( unsigned int iIndex ) { return AGKCommand652( iIndex ); }
		static inline float GetRawTouchCurrentY( unsigned int iIndex ) { return AGKCommand653( iIndex ); }
		static inline float GetRawTouchLastX( unsigned int iIndex ) { return AGKCommand654( iIndex ); }
		static inline float GetRawTouchLastY( unsigned int iIndex ) { return AGKCommand655( iIndex ); }
		static inline int GetRawTouchReleased( unsigned int iIndex ) { return AGKCommand656( iIndex ); }
		static inline void SetRawTouchValue( unsigned int iIndex, int value ) { AGKCommand657( iIndex, value ); }
		static inline int GetRawTouchValue( unsigned int iIndex ) { return AGKCommand658( iIndex ); }
		static inline float GetRawTouchTime( unsigned int iIndex ) { return AGKCommand659( iIndex ); }
		static inline void LoadSound( unsigned int iID, const char* sFilename ) { AGKCommand660( iID, sFilename ); }
		static inline unsigned int LoadSound( const char* sFilename ) { return AGKCommand661( sFilename ); }
		static inline void LoadSoundOGG( unsigned int iID, const char* sFilename ) { AGKCommand662( iID, sFilename ); }
		static inline unsigned int LoadSoundOGG( const char* sFilename ) { return AGKCommand663( sFilename ); }
		static inline void SaveSound( unsigned int iID, const char* sFilename ) { AGKCommand664( iID, sFilename ); }
		static inline unsigned int PlaySound( unsigned int iID ) { return AGKCommand665( iID ); }
		static inline unsigned int PlaySound( unsigned int iID, int iVol ) { return AGKCommand666( iID, iVol ); }
		static inline unsigned int PlaySound( unsigned int iID, int iVol, int iLoop ) { return AGKCommand667( iID, iVol, iLoop ); }
		static inline unsigned int PlaySound( unsigned int iID, int iVol, int iLoop, int iPriority ) { return AGKCommand668( iID, iVol, iLoop, iPriority ); }
		static inline void StopSound( unsigned int iID ) { AGKCommand669( iID ); }
		static inline void DeleteSound( unsigned int iID ) { AGKCommand670( iID ); }
		static inline unsigned int GetSoundExists( unsigned int iID ) { return AGKCommand671( iID ); }
		static inline unsigned int GetSoundInstances( unsigned int iID ) { return AGKCommand672( iID ); }
		static inline unsigned int GetSoundsPlaying( unsigned int iID ) { return AGKCommand673( iID ); }
		static inline void SetSoundSystemVolume( int iVol ) { AGKCommand674( iVol ); }
		static inline float GetSoundMaxRate(  ) { return AGKCommand675(  ); }
		static inline float GetSoundMinRate(  ) { return AGKCommand676(  ); }
		static inline int GetSoundInstancePlaying( unsigned int iID ) { return AGKCommand677( iID ); }
		static inline int GetSoundInstanceVolume( unsigned int iID ) { return AGKCommand678( iID ); }
		static inline float GetSoundInstanceRate( unsigned int iID ) { return AGKCommand679( iID ); }
		static inline void SetSoundInstanceVolume( unsigned int iID, int vol ) { AGKCommand680( iID, vol ); }
		static inline void SetSoundInstanceRate( unsigned int iID, float rate ) { AGKCommand681( iID, rate ); }
		static inline void SetSoundInstanceBalance( unsigned int iID, float balance ) { AGKCommand682( iID, balance ); }
		static inline int GetSoundInstanceLoopCount( unsigned int iID ) { return AGKCommand683( iID ); }
		static inline void StopSoundInstance( unsigned int iID ) { AGKCommand684( iID ); }
		static inline unsigned int LoadMusic( const char * sFile ) { return AGKCommand685( sFile ); }
		static inline void LoadMusic( unsigned int iID, const char * sFile ) { AGKCommand686( iID, sFile ); }
		static inline void SetMusicFileVolume( unsigned int ID, int vol ) { AGKCommand687( ID, vol ); }
		static inline void PlayMusic(  ) { AGKCommand688(  ); }
		static inline void PlayMusic( unsigned int iID ) { AGKCommand689( iID ); }
		static inline void PlayMusic( unsigned int iID, int bLoop ) { AGKCommand690( iID, bLoop ); }
		static inline void PlayMusic( unsigned int iID, int bLoop, unsigned int iStartID, unsigned int iEndID ) { AGKCommand691( iID, bLoop, iStartID, iEndID ); }
		static inline void PauseMusic(  ) { AGKCommand692(  ); }
		static inline void ResumeMusic(  ) { AGKCommand693(  ); }
		static inline void StopMusic(  ) { AGKCommand694(  ); }
		static inline void DeleteMusic( unsigned int iID ) { AGKCommand695( iID ); }
		static inline int GetMusicPlaying(  ) { return AGKCommand696(  ); }
		static inline void SetMusicSystemVolume( int iVol ) { AGKCommand697( iVol ); }
		static inline unsigned int GetMusicExists( unsigned int iID ) { return AGKCommand698( iID ); }
		static inline float GetMusicDuration( unsigned int iID ) { return AGKCommand699( iID ); }
		static inline float GetMusicPosition(  ) { return AGKCommand700(  ); }
		static inline void SeekMusic( float seconds, int mode ) { AGKCommand701( seconds, mode ); }
		static inline unsigned int LoadMusicOGG( const char* sFile ) { return AGKCommand702( sFile ); }
		static inline void LoadMusicOGG( unsigned int musicID, const char* sFile ) { AGKCommand703( musicID, sFile ); }
		static inline int GetMusicExistsOGG( unsigned int musicID ) { return AGKCommand704( musicID ); }
		static inline void SetMusicVolumeOGG( unsigned int musicID, int vol ) { AGKCommand705( musicID, vol ); }
		static inline void PlayMusicOGG( unsigned int musicID ) { AGKCommand706( musicID ); }
		static inline void PlayMusicOGG( unsigned int musicID, int iLoop ) { AGKCommand707( musicID, iLoop ); }
		static inline void PauseMusicOGG( unsigned int musicID ) { AGKCommand708( musicID ); }
		static inline void ResumeMusicOGG( unsigned int musicID ) { AGKCommand709( musicID ); }
		static inline void StopMusicOGG( unsigned int musicID ) { AGKCommand710( musicID ); }
		static inline void DeleteMusicOGG( unsigned int musicID ) { AGKCommand711( musicID ); }
		static inline int GetMusicPlayingOGG( unsigned int musicID ) { return AGKCommand712( musicID ); }
		static inline int GetMusicLoopCountOGG( unsigned int musicID ) { return AGKCommand713( musicID ); }
		static inline float GetMusicDurationOGG( unsigned int musicID ) { return AGKCommand714( musicID ); }
		static inline float GetMusicPositionOGG( unsigned int musicID ) { return AGKCommand715( musicID ); }
		static inline void SeekMusicOGG( unsigned int musicID, float seconds, int mode ) { AGKCommand716( musicID, seconds, mode ); }
		static inline void SetMusicSystemVolumeOGG( int vol ) { AGKCommand717( vol ); }
		static inline void SetMusicLoopTimesOGG( unsigned int musicID, float startTime, float endTime ) { AGKCommand718( musicID, startTime, endTime ); }
		static inline void SetMusicLoopCountOGG( unsigned int musicID, int loop ) { AGKCommand719( musicID, loop ); }
		static inline void RecordSound( const char* szFilename ) { AGKCommand720( szFilename ); }
		static inline void StopSoundRecording(  ) { AGKCommand721(  ); }
		static inline int IsSoundRecording(  ) { return AGKCommand722(  ); }
		static inline void DeleteFile( const char * szFile ) { AGKCommand723( szFile ); }
		static inline int GetFileExists( const char * szFile ) { return AGKCommand724( szFile ); }
		static inline char* ChooseRawFile( const char* ext ) { return AGKCommand725( ext ); }
		static inline char* ChooseRawFile( const char* ext, int returnFullPath ) { return AGKCommand726( ext, returnFullPath ); }
		static inline char* SimplifyPath( const char * szPath ) { return AGKCommand727( szPath ); }
		static inline char* JoinPaths( const char * szPath, const char * szPath2 ) { return AGKCommand728( szPath, szPath2 ); }
		static inline int IsAbsolutePath( const char * szPath ) { return AGKCommand729( szPath ); }
		static inline int CountWindowsDrives(  ) { return AGKCommand730(  ); }
		static inline char* GetWindowsDrive( int index ) { return AGKCommand731( index ); }
		static inline unsigned int OpenRawFolder( const char * szPath ) { return AGKCommand732( szPath ); }
		static inline void CloseRawFolder( unsigned int ID ) { AGKCommand733( ID ); }
		static inline int GetRawFolderNumFiles( unsigned int ID ) { return AGKCommand734( ID ); }
		static inline int GetRawFolderNumFolders( unsigned int ID ) { return AGKCommand735( ID ); }
		static inline char* GetRawFolderFileName( unsigned int ID, int index ) { return AGKCommand736( ID, index ); }
		static inline char* GetRawFolderFolderName( unsigned int ID, int index ) { return AGKCommand737( ID, index ); }
		static inline void OpenToWrite( unsigned int ID, const char * szFile, int append ) { AGKCommand738( ID, szFile, append ); }
		static inline void OpenToWrite( unsigned int ID, const char * szFile ) { AGKCommand739( ID, szFile ); }
		static inline unsigned int OpenToWrite( const char * szFile, int append ) { return AGKCommand740( szFile, append ); }
		static inline unsigned int OpenToWrite( const char * szFile ) { return AGKCommand741( szFile ); }
		static inline unsigned int OpenToRead( const char * szFile ) { return AGKCommand742( szFile ); }
		static inline void OpenToRead( unsigned int ID, const char * szFile ) { AGKCommand743( ID, szFile ); }
		static inline int FileIsOpen( unsigned int iFileID ) { return AGKCommand744( iFileID ); }
		static inline void CloseFile( unsigned int iFileID ) { AGKCommand745( iFileID ); }
		static inline int FileEOF( unsigned int iFileID ) { return AGKCommand746( iFileID ); }
		static inline int GetFileSize( unsigned int iFileID ) { return AGKCommand747( iFileID ); }
		static inline int GetFilePos( unsigned int iFileID ) { return AGKCommand748( iFileID ); }
		static inline void SetFilePos( unsigned int iFileID, int pos ) { AGKCommand749( iFileID, pos ); }
		static inline void WriteByte( unsigned int iFileID, int b ) { AGKCommand750( iFileID, b ); }
		static inline void WriteInteger( unsigned int iFileID, int i ) { AGKCommand751( iFileID, i ); }
		static inline void WriteFloat( unsigned int iFileID, float f ) { AGKCommand752( iFileID, f ); }
		static inline void WriteString( unsigned int iFileID, const char* str ) { AGKCommand753( iFileID, str ); }
		static inline void WriteString2( unsigned int iFileID, const char* str ) { AGKCommand754( iFileID, str ); }
		static inline void WriteLine( unsigned int iFileID, const char* str ) { AGKCommand755( iFileID, str ); }
		static inline int ReadByte( unsigned int iFileID ) { return AGKCommand756( iFileID ); }
		static inline int ReadInteger( unsigned int iFileID ) { return AGKCommand757( iFileID ); }
		static inline float ReadFloat( unsigned int iFileID ) { return AGKCommand758( iFileID ); }
		static inline char* ReadString( unsigned int iFileID ) { return AGKCommand759( iFileID ); }
		static inline char* ReadString2( unsigned int iFileID ) { return AGKCommand760( iFileID ); }
		static inline char* ReadLine( unsigned int iFileID ) { return AGKCommand761( iFileID ); }
		static inline char* Str( int valueInt ) { return AGKCommand762( valueInt ); }
		static inline char* Str( float valueFloat ) { return AGKCommand763( valueFloat ); }
		static inline char* Str( float valueFloat, unsigned int decimals ) { return AGKCommand764( valueFloat, decimals ); }
		static inline char* Bin( int i ) { return AGKCommand765( i ); }
		static inline char* Hex( int i ) { return AGKCommand766( i ); }
		static inline char* StringToBase64( const char* input ) { return AGKCommand767( input ); }
		static inline char* HexToBase64( const char* input ) { return AGKCommand768( input ); }
		static inline int Val( const char* str ) { return AGKCommand769( str ); }
		static inline int Val( const char* str, int base ) { return AGKCommand770( str, base ); }
		static inline float ValFloat( const char* str ) { return AGKCommand771( str ); }
		static inline char* Left( const char* strin, unsigned int count ) { return AGKCommand772( strin, count ); }
		static inline char* Right( const char* strin, unsigned int count ) { return AGKCommand773( strin, count ); }
		static inline char* Mid( const char* strin, unsigned int position, int length ) { return AGKCommand774( strin, position, length ); }
		static inline unsigned int Asc( const char* strin ) { return AGKCommand775( strin ); }
		static inline unsigned int Len( const char* strin ) { return AGKCommand776( strin ); }
		static inline unsigned int ByteLen( const char* strin ) { return AGKCommand777( strin ); }
		static inline char* Chr( unsigned int asciivalue ) { return AGKCommand778( asciivalue ); }
		static inline char* Lower( const char* strin ) { return AGKCommand779( strin ); }
		static inline char* Upper( const char* strin ) { return AGKCommand780( strin ); }
		static inline char* Spaces( unsigned int length ) { return AGKCommand781( length ); }
		static inline int FindStringCount( const char* str, const char* findStr ) { return AGKCommand782( str, findStr ); }
		static inline int FindStringCount( const char* str, const char* findStr, int ignoreCase, int start ) { return AGKCommand783( str, findStr, ignoreCase, start ); }
		static inline int FindString( const char* str, const char* findStr ) { return AGKCommand784( str, findStr ); }
		static inline int FindString( const char* str, const char* findStr, int ignoreCase, int start ) { return AGKCommand785( str, findStr, ignoreCase, start ); }
		static inline int FindStringReverse( const char* str, const char* findStr ) { return AGKCommand786( str, findStr ); }
		static inline int FindStringReverse( const char* str, const char* findStr, int ignoreCase, int start ) { return AGKCommand787( str, findStr, ignoreCase, start ); }
		static inline int CompareString( const char* str, const char* str2 ) { return AGKCommand788( str, str2 ); }
		static inline int CompareString( const char* str, const char* str2, int ignoreCase, int maxChars ) { return AGKCommand789( str, str2, ignoreCase, maxChars ); }
		static inline char* ReplaceString( const char* str, const char* find, const char* replace, int qty ) { return AGKCommand790( str, find, replace, qty ); }
		static inline char* StripString( const char* str, const char* chars ) { return AGKCommand791( str, chars ); }
		static inline char* TrimString( const char* str, const char* chars ) { return AGKCommand792( str, chars ); }
		static inline char* TruncateString( const char* str, const char* character ) { return AGKCommand793( str, character ); }
		static inline int CountStringTokens( const char* str, const char* delimiters ) { return AGKCommand794( str, delimiters ); }
		static inline char* GetStringToken( const char* str, const char* delimiters, int token ) { return AGKCommand795( str, delimiters, token ); }
		static inline int CountStringTokens2( const char* str, const char* delimiter ) { return AGKCommand796( str, delimiter ); }
		static inline char* GetStringToken2( const char* str, const char* delimiter, int token ) { return AGKCommand797( str, delimiter, token ); }
		static inline char* GetCurrentDir(  ) { return AGKCommand798(  ); }
		static inline char* GetFolder(  ) { return AGKCommand799(  ); }
		static inline int SetFolder( const char* str ) { return AGKCommand800( str ); }
		static inline char* GetFirstFolder(  ) { return AGKCommand801(  ); }
		static inline char* GetFirstFolder( int mode ) { return AGKCommand802( mode ); }
		static inline char* GetNextFolder(  ) { return AGKCommand803(  ); }
		static inline int GetFolderCount(  ) { return AGKCommand804(  ); }
		static inline int GetFolderCount( int mode ) { return AGKCommand805( mode ); }
		static inline char* GetFirstFile(  ) { return AGKCommand806(  ); }
		static inline char* GetFirstFile( int mode ) { return AGKCommand807( mode ); }
		static inline char* GetNextFile(  ) { return AGKCommand808(  ); }
		static inline int GetFileCount(  ) { return AGKCommand809(  ); }
		static inline int GetFileCount( int mode ) { return AGKCommand810( mode ); }
		static inline unsigned int CreateBroadcastListener( unsigned int port ) { return AGKCommand811( port ); }
		static inline unsigned int CreateBroadcastListener( const char * szIP, unsigned int port ) { return AGKCommand812( szIP, port ); }
		static inline unsigned int GetBroadcastMessage( unsigned int iID ) { return AGKCommand813( iID ); }
		static inline void DeleteBroadcastListener( unsigned int iID ) { AGKCommand814( iID ); }
		static inline unsigned int ConnectSocket( const char * szIP, int port, int timeout ) { return AGKCommand815( szIP, port, timeout ); }
		static inline int ConnectSocket( unsigned int socketID, const char * szIP, int port, int timeout ) { return AGKCommand816( socketID, szIP, port, timeout ); }
		static inline int GetSocketConnected( unsigned int socketID ) { return AGKCommand817( socketID ); }
		static inline int GetSocketExists( unsigned int socketID ) { return AGKCommand818( socketID ); }
		static inline void DeleteSocket( unsigned int socketID ) { AGKCommand819( socketID ); }
		static inline char* GetSocketRemoteIP( unsigned int socketID ) { return AGKCommand820( socketID ); }
		static inline int FlushSocket( unsigned int socketID ) { return AGKCommand821( socketID ); }
		static inline int GetSocketBytesAvailable( unsigned int socketID ) { return AGKCommand822( socketID ); }
		static inline int GetSocketByte( unsigned int socketID ) { return AGKCommand823( socketID ); }
		static inline int GetSocketInteger( unsigned int socketID ) { return AGKCommand824( socketID ); }
		static inline float GetSocketFloat( unsigned int socketID ) { return AGKCommand825( socketID ); }
		static inline char* GetSocketString( unsigned int socketID ) { return AGKCommand826( socketID ); }
		static inline int SendSocketByte( unsigned int socketID, int value ) { return AGKCommand827( socketID, value ); }
		static inline int SendSocketInteger( unsigned int socketID, int value ) { return AGKCommand828( socketID, value ); }
		static inline int SendSocketFloat( unsigned int socketID, float value ) { return AGKCommand829( socketID, value ); }
		static inline int SendSocketString( unsigned int socketID, const char * value ) { return AGKCommand830( socketID, value ); }
		static inline unsigned int CreateSocketListener( const char * szIP, int port ) { return AGKCommand831( szIP, port ); }
		static inline int CreateSocketListener( unsigned int listenerID, const char * szIP, int port ) { return AGKCommand832( listenerID, szIP, port ); }
		static inline unsigned int GetSocketListenerConnection( unsigned int listenerID ) { return AGKCommand833( listenerID ); }
		static inline void DeleteSocketListener( unsigned int listenerID ) { AGKCommand834( listenerID ); }
		static inline unsigned int HostNetwork( const char * szNetworkName, const char * szMyName, int port ) { return AGKCommand835( szNetworkName, szMyName, port ); }
		static inline unsigned int HostNetwork( const char * szNetworkName, const char * szMyName, int port, int portv6 ) { return AGKCommand836( szNetworkName, szMyName, port, portv6 ); }
		static inline void SetNetworkNoMoreClients( unsigned int iNetID ) { AGKCommand837( iNetID ); }
		static inline unsigned int JoinNetwork( const char * szNetworkName, const char * szMyName ) { return AGKCommand838( szNetworkName, szMyName ); }
		static inline unsigned int JoinNetwork( const char * szIP, unsigned int port, const char * szMyName ) { return AGKCommand839( szIP, port, szMyName ); }
		static inline unsigned int IsNetworkActive( unsigned int iNetID ) { return AGKCommand840( iNetID ); }
		static inline char* GetDeviceIP(  ) { return AGKCommand841(  ); }
		static inline char* GetDeviceIPv6(  ) { return AGKCommand842(  ); }
		static inline void CloseNetwork( unsigned int iNetID ) { AGKCommand843( iNetID ); }
		static inline void SetNetworkLatency( unsigned int iNetID, unsigned int latency ) { AGKCommand844( iNetID, latency ); }
		static inline unsigned int GetNetworkMyClientID( unsigned int iNetID ) { return AGKCommand845( iNetID ); }
		static inline unsigned int GetNetworkNumClients( unsigned int iNetID ) { return AGKCommand846( iNetID ); }
		static inline unsigned int GetNetworkFirstClient( unsigned int iNetID ) { return AGKCommand847( iNetID ); }
		static inline unsigned int GetNetworkNextClient( unsigned int iNetID ) { return AGKCommand848( iNetID ); }
		static inline unsigned int GetNetworkClientDisconnected( unsigned int iNetID, unsigned int client ) { return AGKCommand849( iNetID, client ); }
		static inline void DeleteNetworkClient( unsigned int iNetID, unsigned int client ) { AGKCommand850( iNetID, client ); }
		static inline char* GetNetworkClientName( unsigned int iNetID, unsigned int client ) { return AGKCommand851( iNetID, client ); }
		static inline float GetNetworkClientPing( unsigned int iNetID, unsigned int client ) { return AGKCommand852( iNetID, client ); }
		static inline unsigned int GetNetworkServerID( unsigned int iNetID ) { return AGKCommand853( iNetID ); }
		static inline void SetNetworkLocalInteger( unsigned int iNetID, const char * name, int i ) { AGKCommand854( iNetID, name, i ); }
		static inline void SetNetworkLocalInteger( unsigned int iNetID, const char * name, int i, int mode ) { AGKCommand855( iNetID, name, i, mode ); }
		static inline void SetNetworkLocalFloat( unsigned int iNetID, const char * name, float f ) { AGKCommand856( iNetID, name, f ); }
		static inline void SetNetworkLocalFloat( unsigned int iNetID, const char * name, float f, int mode ) { AGKCommand857( iNetID, name, f, mode ); }
		static inline int GetNetworkClientInteger( unsigned int iNetID, unsigned int client, const char * name ) { return AGKCommand858( iNetID, client, name ); }
		static inline float GetNetworkClientFloat( unsigned int iNetID, unsigned int client, const char * name ) { return AGKCommand859( iNetID, client, name ); }
		static inline unsigned int CreateNetworkMessage(  ) { return AGKCommand860(  ); }
		static inline void AddNetworkMessageInteger( unsigned int iMsgID, int value ) { AGKCommand861( iMsgID, value ); }
		static inline void AddNetworkMessageFloat( unsigned int iMsgID, float value ) { AGKCommand862( iMsgID, value ); }
		static inline void AddNetworkMessageString( unsigned int iMsgID, const char * value ) { AGKCommand863( iMsgID, value ); }
		static inline char* GetNetworkMessageFromIP( unsigned int iMsgID ) { return AGKCommand864( iMsgID ); }
		static inline unsigned int GetNetworkMessageFromClient( unsigned int iMsgID ) { return AGKCommand865( iMsgID ); }
		static inline int GetNetworkMessageInteger( unsigned int iMsgID ) { return AGKCommand866( iMsgID ); }
		static inline float GetNetworkMessageFloat( unsigned int iMsgID ) { return AGKCommand867( iMsgID ); }
		static inline char* GetNetworkMessageString( unsigned int iMsgID ) { return AGKCommand868( iMsgID ); }
		static inline void DeleteNetworkMessage( unsigned int iMsgID ) { AGKCommand869( iMsgID ); }
		static inline void SendNetworkMessage( unsigned int iNetID, unsigned int toClient, unsigned int iMsgID ) { AGKCommand870( iNetID, toClient, iMsgID ); }
		static inline unsigned int GetNetworkMessage( unsigned int iNetID ) { return AGKCommand871( iNetID ); }
		static inline void SetNetworkClientUserData( unsigned int iNetID, unsigned int client, unsigned int index, int value ) { AGKCommand872( iNetID, client, index, value ); }
		static inline int GetNetworkClientUserData( unsigned int iNetID, unsigned int client, unsigned int index ) { return AGKCommand873( iNetID, client, index ); }
		static inline unsigned int CreateHTTPConnection(  ) { return AGKCommand874(  ); }
		static inline void DeleteHTTPConnection( unsigned int iHTTP ) { AGKCommand875( iHTTP ); }
		static inline unsigned int SetHTTPHost( unsigned int iHTTP, const char * szHost, int iSecure ) { return AGKCommand876( iHTTP, szHost, iSecure ); }
		static inline unsigned int SetHTTPHost( unsigned int iHTTP, const char * szHost, int iSecure, const char * szUser, const char * szPass ) { return AGKCommand877( iHTTP, szHost, iSecure, szUser, szPass ); }
		static inline void CloseHTTPConnection( unsigned int iHTTP ) { AGKCommand878( iHTTP ); }
		static inline void SetHTTPTimeout( unsigned int iHTTP, int milliseconds ) { AGKCommand879( iHTTP, milliseconds ); }
		static inline void SetHTTPVerifyCertificate( unsigned int iHTTP, int mode ) { AGKCommand880( iHTTP, mode ); }
		static inline char* SendHTTPRequest( unsigned int iHTTP, const char * szServerFile ) { return AGKCommand881( iHTTP, szServerFile ); }
		static inline char* SendHTTPRequest( unsigned int iHTTP, const char * szServerFile, const char * szPostData ) { return AGKCommand882( iHTTP, szServerFile, szPostData ); }
		static inline int SendHTTPRequestASync( unsigned int iHTTP, const char * szServerFile ) { return AGKCommand883( iHTTP, szServerFile ); }
		static inline int SendHTTPRequestASync( unsigned int iHTTP, const char * szServerFile, const char * szPostData ) { return AGKCommand884( iHTTP, szServerFile, szPostData ); }
		static inline int SendHTTPFile( unsigned int iHTTP, const char * szServerFile, const char * szPostData, const char * szLocalFile ) { return AGKCommand885( iHTTP, szServerFile, szPostData, szLocalFile ); }
		static inline char* GetHTTPResponse( unsigned int iHTTP ) { return AGKCommand886( iHTTP ); }
		static inline int GetHTTPResponseReady( unsigned int iHTTP ) { return AGKCommand887( iHTTP ); }
		static inline int GetHTTPFile( unsigned int iHTTP, const char * szServerFile, const char * szLocalFile ) { return AGKCommand888( iHTTP, szServerFile, szLocalFile ); }
		static inline int GetHTTPFile( unsigned int iHTTP, const char * szServerFile, const char * szLocalFile, const char * szPostData ) { return AGKCommand889( iHTTP, szServerFile, szLocalFile, szPostData ); }
		static inline int GetHTTPFileComplete( unsigned int iHTTP ) { return AGKCommand890( iHTTP ); }
		static inline float GetHTTPFileProgress( unsigned int iHTTP ) { return AGKCommand891( iHTTP ); }
		static inline void SetErrorMode( int mode ) { AGKCommand892( mode ); }
		static inline char* GetLastError(  ) { return AGKCommand893(  ); }
		static inline int GetErrorOccurred(  ) { return AGKCommand894(  ); }
		static inline void PluginError( const char * szErr ) { AGKCommand895( szErr ); }
		static inline void Message( const char* msg ) { AGKCommand896( msg ); }
		static inline int GetJoystickExists(  ) { return AGKCommand897(  ); }
		static inline float GetRawMouseX(  ) { return AGKCommand898(  ); }
		static inline float GetRawMouseY(  ) { return AGKCommand899(  ); }
		static inline float GetRawMouseWheel(  ) { return AGKCommand900(  ); }
		static inline float GetRawMouseWheelDelta(  ) { return AGKCommand901(  ); }
		static inline int GetRawMouseLeftPressed(  ) { return AGKCommand902(  ); }
		static inline int GetRawMouseLeftState(  ) { return AGKCommand903(  ); }
		static inline int GetRawMouseLeftReleased(  ) { return AGKCommand904(  ); }
		static inline int GetRawMouseRightPressed(  ) { return AGKCommand905(  ); }
		static inline int GetRawMouseRightState(  ) { return AGKCommand906(  ); }
		static inline int GetRawMouseRightReleased(  ) { return AGKCommand907(  ); }
		static inline int GetRawMouseMiddlePressed(  ) { return AGKCommand908(  ); }
		static inline int GetRawMouseMiddleState(  ) { return AGKCommand909(  ); }
		static inline int GetRawMouseMiddleReleased(  ) { return AGKCommand910(  ); }
		static inline int GetAccelerometerExists(  ) { return AGKCommand911(  ); }
		static inline int GetGyroSensorExists(  ) { return AGKCommand912(  ); }
		static inline int GetProximitySensorExists(  ) { return AGKCommand913(  ); }
		static inline int GetLightSensorExists(  ) { return AGKCommand914(  ); }
		static inline int GetMagneticSensorExists(  ) { return AGKCommand915(  ); }
		static inline int GetRotationVectorSensorExists(  ) { return AGKCommand916(  ); }
		static inline float GetRawAccelX(  ) { return AGKCommand917(  ); }
		static inline float GetRawAccelY(  ) { return AGKCommand918(  ); }
		static inline float GetRawAccelZ(  ) { return AGKCommand919(  ); }
		static inline float GetRawGyroVelocityX(  ) { return AGKCommand920(  ); }
		static inline float GetRawGyroVelocityY(  ) { return AGKCommand921(  ); }
		static inline float GetRawGyroVelocityZ(  ) { return AGKCommand922(  ); }
		static inline float GetRawProximityDistance(  ) { return AGKCommand923(  ); }
		static inline float GetRawLightLevel(  ) { return AGKCommand924(  ); }
		static inline float GetRawMagneticX(  ) { return AGKCommand925(  ); }
		static inline float GetRawMagneticY(  ) { return AGKCommand926(  ); }
		static inline float GetRawMagneticZ(  ) { return AGKCommand927(  ); }
		static inline float GetRawRotationVectorX(  ) { return AGKCommand928(  ); }
		static inline float GetRawRotationVectorY(  ) { return AGKCommand929(  ); }
		static inline float GetRawRotationVectorZ(  ) { return AGKCommand930(  ); }
		static inline float GetRawRotationVectorW(  ) { return AGKCommand931(  ); }
		static inline float GetRawRotationVectorX2(  ) { return AGKCommand932(  ); }
		static inline float GetRawRotationVectorY2(  ) { return AGKCommand933(  ); }
		static inline float GetRawRotationVectorZ2(  ) { return AGKCommand934(  ); }
		static inline float GetRawRotationVectorW2(  ) { return AGKCommand935(  ); }
		static inline void CompleteRawJoystickDetection(  ) { AGKCommand936(  ); }
		static inline int GetRawJoystickExists( unsigned int index ) { return AGKCommand937( index ); }
		static inline int GetRawJoystickConnected( unsigned int index ) { return AGKCommand938( index ); }
		static inline float GetRawJoystickX( unsigned int index ) { return AGKCommand939( index ); }
		static inline float GetRawJoystickY( unsigned int index ) { return AGKCommand940( index ); }
		static inline float GetRawJoystickZ( unsigned int index ) { return AGKCommand941( index ); }
		static inline float GetRawJoystickRX( unsigned int index ) { return AGKCommand942( index ); }
		static inline float GetRawJoystickRY( unsigned int index ) { return AGKCommand943( index ); }
		static inline float GetRawJoystickRZ( unsigned int index ) { return AGKCommand944( index ); }
		static inline int GetRawJoystickButtonPressed( unsigned int index, unsigned int button ) { return AGKCommand945( index, button ); }
		static inline int GetRawJoystickButtonState( unsigned int index, unsigned int button ) { return AGKCommand946( index, button ); }
		static inline int GetRawJoystickButtonReleased( unsigned int index, unsigned int button ) { return AGKCommand947( index, button ); }
		static inline void SetRawJoystickDeadZone( float threshold ) { AGKCommand948( threshold ); }
		static inline void AddVirtualJoystick( unsigned int index, float x, float y, float size ) { AGKCommand949( index, x, y, size ); }
		static inline void DeleteVirtualJoystick( unsigned int index ) { AGKCommand950( index ); }
		static inline int GetVirtualJoystickExists( unsigned int index ) { return AGKCommand951( index ); }
		static inline float GetVirtualJoystickX( unsigned int index ) { return AGKCommand952( index ); }
		static inline float GetVirtualJoystickY( unsigned int index ) { return AGKCommand953( index ); }
		static inline void SetVirtualJoystickAlpha( unsigned int index, unsigned int alpha1, unsigned int alpha2 ) { AGKCommand954( index, alpha1, alpha2 ); }
		static inline void SetVirtualJoystickPosition( unsigned int index, float x, float y ) { AGKCommand955( index, x, y ); }
		static inline void SetVirtualJoystickSize( unsigned int index, float size ) { AGKCommand956( index, size ); }
		static inline void SetVirtualJoystickActive( unsigned int index, int active ) { AGKCommand957( index, active ); }
		static inline void SetVirtualJoystickVisible( unsigned int index, int visible ) { AGKCommand958( index, visible ); }
		static inline void SetVirtualJoystickImageInner( unsigned int index, unsigned int imageID ) { AGKCommand959( index, imageID ); }
		static inline void SetVirtualJoystickImageOuter( unsigned int index, unsigned int imageID ) { AGKCommand960( index, imageID ); }
		static inline void SetVirtualJoystickDeadZone( float threshold ) { AGKCommand961( threshold ); }
		static inline void AddVirtualButton( unsigned int index, float x, float y, float size ) { AGKCommand962( index, x, y, size ); }
		static inline void DeleteVirtualButton( unsigned int index ) { AGKCommand963( index ); }
		static inline int GetVirtualButtonExists( unsigned int index ) { return AGKCommand964( index ); }
		static inline int GetVirtualButtonPressed( unsigned int index ) { return AGKCommand965( index ); }
		static inline int GetVirtualButtonReleased( unsigned int index ) { return AGKCommand966( index ); }
		static inline int GetVirtualButtonState( unsigned int index ) { return AGKCommand967( index ); }
		static inline void SetVirtualButtonColor( unsigned int index, unsigned int red, unsigned int green, unsigned int blue ) { AGKCommand968( index, red, green, blue ); }
		static inline void SetVirtualButtonAlpha( unsigned int index, unsigned int alpha ) { AGKCommand969( index, alpha ); }
		static inline void SetVirtualButtonPosition( unsigned int index, float x, float y ) { AGKCommand970( index, x, y ); }
		static inline void SetVirtualButtonSize( unsigned int index, float size ) { AGKCommand971( index, size ); }
		static inline void SetVirtualButtonSize( unsigned int index, float sizeX, float sizeY ) { AGKCommand972( index, sizeX, sizeY ); }
		static inline void SetVirtualButtonActive( unsigned int index, int active ) { AGKCommand973( index, active ); }
		static inline void SetVirtualButtonVisible( unsigned int index, int visible ) { AGKCommand974( index, visible ); }
		static inline void SetVirtualButtonImageUp( unsigned int index, unsigned int imageID ) { AGKCommand975( index, imageID ); }
		static inline void SetVirtualButtonImageDown( unsigned int index, unsigned int imageID ) { AGKCommand976( index, imageID ); }
		static inline void SetVirtualButtonText( unsigned int index, const char * str ) { AGKCommand977( index, str ); }
		static inline int GetRawKeyPressed( unsigned int key ) { return AGKCommand978( key ); }
		static inline int GetRawKeyState( unsigned int key ) { return AGKCommand979( key ); }
		static inline int GetRawKeyReleased( unsigned int key ) { return AGKCommand980( key ); }
		static inline int GetRawLastKey(  ) { return AGKCommand981(  ); }
		static inline float GetDirectionX(  ) { return AGKCommand982(  ); }
		static inline float GetDirectionY(  ) { return AGKCommand983(  ); }
		static inline float GetDirectionAngle(  ) { return AGKCommand984(  ); }
		static inline float GetDirectionSpeed(  ) { return AGKCommand985(  ); }
		static inline int GetPointerPressed(  ) { return AGKCommand986(  ); }
		static inline int GetPointerReleased(  ) { return AGKCommand987(  ); }
		static inline int GetPointerState(  ) { return AGKCommand988(  ); }
		static inline float GetPointerX(  ) { return AGKCommand989(  ); }
		static inline float GetPointerY(  ) { return AGKCommand990(  ); }
		static inline float GetJoystickX(  ) { return AGKCommand991(  ); }
		static inline float GetJoystickY(  ) { return AGKCommand992(  ); }
		static inline void SetJoystickDeadZone( float threshold ) { AGKCommand993( threshold ); }
		static inline void SetJoystickScreenPosition( float x, float y, float size ) { AGKCommand994( x, y, size ); }
		static inline int GetButtonPressed( unsigned int index ) { return AGKCommand995( index ); }
		static inline int GetButtonState( unsigned int index ) { return AGKCommand996( index ); }
		static inline int GetButtonReleased( unsigned int index ) { return AGKCommand997( index ); }
		static inline void SetButtonScreenPosition( unsigned int index, float x, float y, float size ) { AGKCommand998( index, x, y, size ); }
		static inline void StartTextInput(  ) { AGKCommand999(  ); }
		static inline void StartTextInput( const char* initial ) { AGKCommand1000( initial ); }
		static inline void StopTextInput(  ) { AGKCommand1001(  ); }
		static inline int GetTextInputState(  ) { return AGKCommand1002(  ); }
		static inline int GetTextInputCompleted(  ) { return AGKCommand1003(  ); }
		static inline int GetTextInputCancelled(  ) { return AGKCommand1004(  ); }
		static inline char* GetTextInput(  ) { return AGKCommand1005(  ); }
		static inline unsigned int GetLastChar(  ) { return AGKCommand1006(  ); }
		static inline void SetCursorBlinkTime( float seconds ) { AGKCommand1007( seconds ); }
		static inline void SetTextInputMaxChars( int max ) { AGKCommand1008( max ); }
		static inline void CreateEditBox( unsigned int index ) { AGKCommand1009( index ); }
		static inline unsigned int CreateEditBox(  ) { return AGKCommand1010(  ); }
		static inline int GetEditBoxExists( unsigned int index ) { return AGKCommand1011( index ); }
		static inline void DeleteEditBox( unsigned int index ) { AGKCommand1012( index ); }
		static inline int GetEditBoxHasFocus( unsigned int index ) { return AGKCommand1013( index ); }
		static inline unsigned int GetCurrentEditBox(  ) { return AGKCommand1014(  ); }
		static inline void SetEditBoxPosition( unsigned int index, float x, float y ) { AGKCommand1015( index, x, y ); }
		static inline void SetEditBoxSize( unsigned int index, float width, float height ) { AGKCommand1016( index, width, height ); }
		static inline void SetEditBoxDepth( unsigned int index, int depth ) { AGKCommand1017( index, depth ); }
		static inline void SetEditBoxBorderSize( unsigned int index, float size ) { AGKCommand1018( index, size ); }
		static inline void SetEditBoxBorderColor( unsigned int index, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha ) { AGKCommand1019( index, red, green, blue, alpha ); }
		static inline void SetEditBoxBackgroundColor( unsigned int index, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha ) { AGKCommand1020( index, red, green, blue, alpha ); }
		static inline void SetEditBoxText( unsigned int index, const char * str ) { AGKCommand1021( index, str ); }
		static inline void SetEditBoxTextColor( unsigned int index, unsigned int red, unsigned int green, unsigned int blue ) { AGKCommand1022( index, red, green, blue ); }
		static inline void SetEditBoxCursorColor( unsigned int index, unsigned int red, unsigned int green, unsigned int blue ) { AGKCommand1023( index, red, green, blue ); }
		static inline void SetEditBoxFontImage( unsigned int index, unsigned int image ) { AGKCommand1024( index, image ); }
		static inline void SetEditBoxExtendedFontImage( unsigned int index, unsigned int image ) { AGKCommand1025( index, image ); }
		static inline void SetEditBoxFont( unsigned int index, unsigned int fontID ) { AGKCommand1026( index, fontID ); }
		static inline void SetEditBoxTextSize( unsigned int index, float size ) { AGKCommand1027( index, size ); }
		static inline void SetEditBoxCursorPosition( unsigned int index, int pos ) { AGKCommand1028( index, pos ); }
		static inline void SetEditBoxFocus( unsigned int index, int focus ) { AGKCommand1029( index, focus ); }
		static inline void SetEditBoxActive( unsigned int index, int active ) { AGKCommand1030( index, active ); }
		static inline void SetEditBoxVisible( unsigned int index, int visible ) { AGKCommand1031( index, visible ); }
		static inline void SetEditBoxBorderImage( unsigned int index, unsigned int image ) { AGKCommand1032( index, image ); }
		static inline void SetEditBoxBackgroundImage( unsigned int index, unsigned int image ) { AGKCommand1033( index, image ); }
		static inline void SetEditBoxCursorBlinkTime( unsigned int index, float time ) { AGKCommand1034( index, time ); }
		static inline void SetEditBoxCursorWidth( unsigned int index, float width ) { AGKCommand1035( index, width ); }
		static inline void SetEditBoxMaxChars( unsigned int index, unsigned int max ) { AGKCommand1036( index, max ); }
		static inline void SetEditBoxMaxLines( unsigned int index, unsigned int max ) { AGKCommand1037( index, max ); }
		static inline void SetEditBoxMultiLine( unsigned int index, int multiline ) { AGKCommand1038( index, multiline ); }
		static inline void SetEditBoxScissor( unsigned int index, float x, float y, float x2, float y2 ) { AGKCommand1039( index, x, y, x2, y2 ); }
		static inline void SetEditBoxPasswordMode( unsigned int index, int mode ) { AGKCommand1040( index, mode ); }
		static inline void SetEditBoxUseAlternateInput( unsigned int index, int mode ) { AGKCommand1041( index, mode ); }
		static inline void SetEditBoxWrapMode( unsigned int index, int mode ) { AGKCommand1042( index, mode ); }
		static inline void FixEditBoxToScreen( unsigned int index, int fix ) { AGKCommand1043( index, fix ); }
		static inline char* GetEditBoxText( unsigned int index ) { return AGKCommand1044( index ); }
		static inline float GetEditBoxX( unsigned int index ) { return AGKCommand1045( index ); }
		static inline float GetEditBoxY( unsigned int index ) { return AGKCommand1046( index ); }
		static inline float GetEditBoxWidth( unsigned int index ) { return AGKCommand1047( index ); }
		static inline float GetEditBoxHeight( unsigned int index ) { return AGKCommand1048( index ); }
		static inline int GetEditBoxChanged( unsigned int index ) { return AGKCommand1049( index ); }
		static inline int GetEditBoxActive( unsigned int index ) { return AGKCommand1050( index ); }
		static inline int GetEditBoxVisible( unsigned int index ) { return AGKCommand1051( index ); }
		static inline int GetEditBoxLines( unsigned int index ) { return AGKCommand1052( index ); }
		static inline int GetEditBoxCursorPosition( unsigned int index ) { return AGKCommand1053( index ); }
		static inline unsigned int GetManagedSpriteDrawnCount(  ) { return AGKCommand1054(  ); }
		static inline unsigned int GetManagedSpriteCount(  ) { return AGKCommand1055(  ); }
		static inline unsigned int GetManagedSpriteSortedCount(  ) { return AGKCommand1056(  ); }
		static inline unsigned int GetPixelsDrawn(  ) { return AGKCommand1057(  ); }
		static inline unsigned int GetManagedSpriteDrawCalls(  ) { return AGKCommand1058(  ); }
		static inline unsigned int GetParticleDrawnPointCount(  ) { return AGKCommand1059(  ); }
		static inline unsigned int GetParticleDrawnQuadCount(  ) { return AGKCommand1060(  ); }
		static inline float GetUpdateTime(  ) { return AGKCommand1061(  ); }
		static inline float GetPhysicsTime(  ) { return AGKCommand1062(  ); }
		static inline float GetDrawingSetupTime(  ) { return AGKCommand1063(  ); }
		static inline float GetDrawingTime(  ) { return AGKCommand1064(  ); }
		static inline unsigned int GetLoadedImages(  ) { return AGKCommand1065(  ); }
		static inline unsigned int GetUnassignedImages(  ) { return AGKCommand1066(  ); }
		static inline char* GetUnassignedImageFileName( unsigned int index ) { return AGKCommand1067( index ); }
		static inline float GetImageMemoryUsage(  ) { return AGKCommand1068(  ); }
		static inline int GetLeapYear( int year ) { return AGKCommand1069( year ); }
		static inline int GetUnixFromDate( int year, int month, int days, int hours, int minutes, int seconds ) { return AGKCommand1070( year, month, days, hours, minutes, seconds ); }
		static inline int GetYearFromUnix( int unixtime ) { return AGKCommand1071( unixtime ); }
		static inline int GetMonthFromUnix( int unixtime ) { return AGKCommand1072( unixtime ); }
		static inline int GetDaysFromUnix( int unixtime ) { return AGKCommand1073( unixtime ); }
		static inline int GetHoursFromUnix( int unixtime ) { return AGKCommand1074( unixtime ); }
		static inline int GetMinutesFromUnix( int unixtime ) { return AGKCommand1075( unixtime ); }
		static inline int GetSecondsFromUnix( int unixtime ) { return AGKCommand1076( unixtime ); }
		static inline void SetInneractiveDetails( const char* szCode ) { AGKCommand1077( szCode ); }
		static inline void SetAdMobDetails( const char* szID ) { AGKCommand1078( szID ); }
		static inline void SetAdMobRewardAdDetails( const char* szID ) { AGKCommand1079( szID ); }
		static inline void SetChartboostDetails( const char* szKey1, const char* szKey2 ) { AGKCommand1080( szKey1, szKey2 ); }
		static inline void SetAmazonAdDetails( const char* szKey ) { AGKCommand1081( szKey ); }
		static inline void SetAmazonAdTesting( int mode ) { AGKCommand1082( mode ); }
		static inline void SetAdMobTesting( int mode ) { AGKCommand1083( mode ); }
		static inline void ShowFullscreenAdvertAdMob(  ) { AGKCommand1084(  ); }
		static inline void ShowFullscreenAdvertChartboost(  ) { AGKCommand1085(  ); }
		static inline void ShowFullscreenAdvertAmazon(  ) { AGKCommand1086(  ); }
		static inline int GetFullscreenAdvertLoadedAdMob(  ) { return AGKCommand1087(  ); }
		static inline int GetFullscreenAdvertLoadedChartboost(  ) { return AGKCommand1088(  ); }
		static inline int GetFullscreenAdvertLoadedAmazon(  ) { return AGKCommand1089(  ); }
		static inline void ShowRewardAdAdMob(  ) { AGKCommand1090(  ); }
		static inline int GetRewardAdLoadedAdMob(  ) { return AGKCommand1091(  ); }
		static inline int GetRewardAdRewardedAdMob(  ) { return AGKCommand1092(  ); }
		static inline void ResetRewardAdMob(  ) { AGKCommand1093(  ); }
		static inline void ShowRewardAdChartboost(  ) { AGKCommand1094(  ); }
		static inline void CacheRewardAdChartboost(  ) { AGKCommand1095(  ); }
		static inline int GetRewardAdLoadedChartboost(  ) { return AGKCommand1096(  ); }
		static inline int GetRewardAdRewardedChartboost(  ) { return AGKCommand1097(  ); }
		static inline void ResetRewardChartboost(  ) { AGKCommand1098(  ); }
		static inline void CreateFullscreenAdvert(  ) { AGKCommand1099(  ); }
		static inline void CreateAdvert( int type, int horz, int vert, int test ) { AGKCommand1100( type, horz, vert, test ); }
		static inline void CreateAdvertEx( int type, int horz, int vert, int test, float offsetx, float offsety ) { AGKCommand1101( type, horz, vert, test, offsetx, offsety ); }
		static inline void SetAdvertPosition( float x, float y, float width ) { AGKCommand1102( x, y, width ); }
		static inline void SetAdvertLocation( int horz, int vert, float width ) { AGKCommand1103( horz, vert, width ); }
		static inline void SetAdvertLocationEx( int horz, int vert, float offsetx, float offsety, float width ) { AGKCommand1104( horz, vert, offsetx, offsety, width ); }
		static inline void SetAdvertVisible( int iVisible ) { AGKCommand1105( iVisible ); }
		static inline void RequestAdvertRefresh(  ) { AGKCommand1106(  ); }
		static inline void DeleteAdvert(  ) { AGKCommand1107(  ); }
		static inline void CreateZip( unsigned int zipID, const char* filename ) { AGKCommand1108( zipID, filename ); }
		static inline unsigned int CreateZip( const char* filename ) { return AGKCommand1109( filename ); }
		static inline void AddZipEntry( unsigned int zipID, const char* path, const char* zipPath ) { AGKCommand1110( zipID, path, zipPath ); }
		static inline void CloseZip( unsigned int zipID ) { AGKCommand1111( zipID ); }
		static inline void ExtractZip( const char* zipfilename, const char* path ) { AGKCommand1112( zipfilename, path ); }
		static inline void Log( const char * szMessage ) { AGKCommand1113( szMessage ); }
		static inline void RateApp( const char* szID ) { AGKCommand1114( szID ); }
		static inline void RateApp( const char* szID, const char* szTitle ) { AGKCommand1115( szID, szTitle ); }
		static inline void RateApp( const char* szID, const char* szTitle, const char* szMessage ) { AGKCommand1116( szID, szTitle, szMessage ); }
		static inline void RequestAppReview(  ) { AGKCommand1117(  ); }
		static inline void InAppPurchaseSetKeys( const char* szData1, const char* szData2 ) { AGKCommand1118( szData1, szData2 ); }
		static inline void InAppPurchaseSetTitle( const char* szTitle ) { AGKCommand1119( szTitle ); }
		static inline void InAppPurchaseAddProductID( const char* szID, int type ) { AGKCommand1120( szID, type ); }
		static inline void InAppPurchaseSetup(  ) { AGKCommand1121(  ); }
		static inline int GetInAppPurchaseAvailable( int iID ) { return AGKCommand1122( iID ); }
		static inline void InAppPurchaseActivate( int iID ) { AGKCommand1123( iID ); }
		static inline char* GetInAppPurchaseLocalPrice( int iID ) { return AGKCommand1124( iID ); }
		static inline char* GetInAppPurchaseDescription( int iID ) { return AGKCommand1125( iID ); }
		static inline int GetInAppPurchaseState(  ) { return AGKCommand1126(  ); }
		static inline void InAppPurchaseRestore(  ) { AGKCommand1127(  ); }
		static inline char* GetInAppPurchaseSignature( int iID ) { return AGKCommand1128( iID ); }
		static inline void TwitterSetup( const char* szKey, const char* szSecret ) { AGKCommand1129( szKey, szSecret ); }
		static inline void TwitterMessage( const char* szMessage ) { AGKCommand1130( szMessage ); }
		static inline void FacebookSetup( const char* szID ) { AGKCommand1131( szID ); }
		static inline int GetFacebookLoggedIn(  ) { return AGKCommand1132(  ); }
		static inline char* FacebookGetUserID(  ) { return AGKCommand1133(  ); }
		static inline char* FacebookGetUserName(  ) { return AGKCommand1134(  ); }
		static inline char* FacebookGetAccessToken(  ) { return AGKCommand1135(  ); }
		static inline void FacebookLogin(  ) { AGKCommand1136(  ); }
		static inline void FacebookLogout(  ) { AGKCommand1137(  ); }
		static inline void FacebookPostOnMyWall( const char* szLink, const char* szPicture, const char* szName, const char* szCaption, const char* szDescription ) { AGKCommand1138( szLink, szPicture, szName, szCaption, szDescription ); }
		static inline void FacebookPostOnFriendsWall( const char* szID, const char* szLink, const char* szPicture, const char* szName, const char* szCaption, const char* szDescription ) { AGKCommand1139( szID, szLink, szPicture, szName, szCaption, szDescription ); }
		static inline void FacebookInviteFriend( const char* szID, const char* szMessage ) { AGKCommand1140( szID, szMessage ); }
		static inline void FacebookShowLikeButton( const char* szURL, int iX, int iY, int iWidth, int iHeight ) { AGKCommand1141( szURL, iX, iY, iWidth, iHeight ); }
		static inline void FacebookDestroyLikeButton(  ) { AGKCommand1142(  ); }
		static inline void FacebookGetFriends(  ) { AGKCommand1143(  ); }
		static inline int FacebookGetFriendsState(  ) { return AGKCommand1144(  ); }
		static inline int FacebookGetFriendsCount(  ) { return AGKCommand1145(  ); }
		static inline char* FacebookGetFriendsName( int iIndex ) { return AGKCommand1146( iIndex ); }
		static inline char* FacebookGetFriendsID( int iIndex ) { return AGKCommand1147( iIndex ); }
		static inline void FacebookDownloadFriendsPhoto( int iIndex ) { AGKCommand1148( iIndex ); }
		static inline int GetFacebookDownloadState(  ) { return AGKCommand1149(  ); }
		static inline char* GetFacebookDownloadFile(  ) { return AGKCommand1150(  ); }
		static inline void NotificationCreate( const char* szDateTime, const char* szMessage, const char* szData ) { AGKCommand1151( szDateTime, szMessage, szData ); }
		static inline int GetNotification(  ) { return AGKCommand1152(  ); }
		static inline char* GetNotificationData(  ) { return AGKCommand1153(  ); }
		static inline void NotificationReset(  ) { AGKCommand1154(  ); }
		static inline void SetLocalNotification( int iID, int datetime, const char * szMessage ) { AGKCommand1155( iID, datetime, szMessage ); }
		static inline void CancelLocalNotification( int iID ) { AGKCommand1156( iID ); }
		static inline int GetLocalNotificationExists( int iID ) { return AGKCommand1157( iID ); }
		static inline int GetLocalNotificationTime( int iID ) { return AGKCommand1158( iID ); }
		static inline char* GetLocalNotificationMessage( int iID ) { return AGKCommand1159( iID ); }
		static inline unsigned int MakeColor( unsigned int red, unsigned int green, unsigned int blue ) { return AGKCommand1160( red, green, blue ); }
		static inline unsigned int GetColorRed( unsigned int color ) { return AGKCommand1161( color ); }
		static inline unsigned int GetColorGreen( unsigned int color ) { return AGKCommand1162( color ); }
		static inline unsigned int GetColorBlue( unsigned int color ) { return AGKCommand1163( color ); }
		static inline void DrawLine( float x, float y, float x2, float y2, unsigned int red, unsigned int green, unsigned int blue ) { AGKCommand1164( x, y, x2, y2, red, green, blue ); }
		static inline void DrawLine( float x, float y, float x2, float y2, unsigned int color1, unsigned int color2 ) { AGKCommand1165( x, y, x2, y2, color1, color2 ); }
		static inline void DrawBox( float x, float y, float x2, float y2, unsigned int color1, unsigned int color2, unsigned int color3, unsigned int color4, int filled ) { AGKCommand1166( x, y, x2, y2, color1, color2, color3, color4, filled ); }
		static inline void DrawEllipse( float x, float y, float radiusx, float radiusy, unsigned int color1, unsigned int color2, int filled ) { AGKCommand1167( x, y, radiusx, radiusy, color1, color2, filled ); }
		static inline unsigned int CreateMemblock( unsigned int size ) { return AGKCommand1168( size ); }
		static inline void CreateMemblock( unsigned int memID, unsigned int size ) { AGKCommand1169( memID, size ); }
		static inline int GetMemblockExists( unsigned int memID ) { return AGKCommand1170( memID ); }
		static inline void DeleteMemblock( unsigned int memID ) { AGKCommand1171( memID ); }
		static inline void CopyMemblock( unsigned int memSrcID, unsigned int memDstID, unsigned int srcOffset, unsigned int dstOffset, unsigned int size ) { AGKCommand1172( memSrcID, memDstID, srcOffset, dstOffset, size ); }
		static inline int GetMemblockSize( unsigned int memID ) { return AGKCommand1173( memID ); }
		static inline int GetMemblockByte( unsigned int memID, unsigned int offset ) { return AGKCommand1174( memID, offset ); }
		static inline int GetMemblockByteSigned( unsigned int memID, unsigned int offset ) { return AGKCommand1175( memID, offset ); }
		static inline int GetMemblockShort( unsigned int memID, unsigned int offset ) { return AGKCommand1176( memID, offset ); }
		static inline int GetMemblockInt( unsigned int memID, unsigned int offset ) { return AGKCommand1177( memID, offset ); }
		static inline float GetMemblockFloat( unsigned int memID, unsigned int offset ) { return AGKCommand1178( memID, offset ); }
		static inline char* GetMemblockString( unsigned int memID, unsigned int offset, unsigned int length ) { return AGKCommand1179( memID, offset, length ); }
		static inline void SetMemblockByte( unsigned int memID, unsigned int offset, int value ) { AGKCommand1180( memID, offset, value ); }
		static inline void SetMemblockByteSigned( unsigned int memID, unsigned int offset, int value ) { AGKCommand1181( memID, offset, value ); }
		static inline void SetMemblockShort( unsigned int memID, unsigned int offset, int value ) { AGKCommand1182( memID, offset, value ); }
		static inline void SetMemblockInt( unsigned int memID, unsigned int offset, int value ) { AGKCommand1183( memID, offset, value ); }
		static inline void SetMemblockFloat( unsigned int memID, unsigned int offset, float value ) { AGKCommand1184( memID, offset, value ); }
		static inline void SetMemblockString( unsigned int memID, unsigned int offset, const char* value ) { AGKCommand1185( memID, offset, value ); }
		static inline void CreateMemblockFromImage( unsigned int memID, unsigned int imageID ) { AGKCommand1186( memID, imageID ); }
		static inline unsigned int CreateMemblockFromImage( unsigned int imageID ) { return AGKCommand1187( imageID ); }
		static inline void CreateImageFromMemblock( unsigned int imageID, unsigned int memID ) { AGKCommand1188( imageID, memID ); }
		static inline unsigned int CreateImageFromMemblock( unsigned int memID ) { return AGKCommand1189( memID ); }
		static inline void CreateMemblockFromSound( unsigned int memID, unsigned int soundID ) { AGKCommand1190( memID, soundID ); }
		static inline unsigned int CreateMemblockFromSound( unsigned int soundID ) { return AGKCommand1191( soundID ); }
		static inline void CreateSoundFromMemblock( unsigned int soundID, unsigned int memID ) { AGKCommand1192( soundID, memID ); }
		static inline unsigned int CreateSoundFromMemblock( unsigned int memID ) { return AGKCommand1193( memID ); }
		static inline void CreateMemblockFromFile( unsigned int memID, const char* filename ) { AGKCommand1194( memID, filename ); }
		static inline unsigned int CreateMemblockFromFile( const char* filename ) { return AGKCommand1195( filename ); }
		static inline void CreateFileFromMemblock( const char* filename, unsigned int memID ) { AGKCommand1196( filename, memID ); }
		static inline void CreateMemblockFromObjectMesh( unsigned int memID, unsigned int objID, unsigned int meshIndex ) { AGKCommand1197( memID, objID, meshIndex ); }
		static inline unsigned int CreateMemblockFromObjectMesh( unsigned int objID, unsigned int meshIndex ) { return AGKCommand1198( objID, meshIndex ); }
		static inline void CreateObjectFromMeshMemblock( unsigned int objID, unsigned int memID ) { AGKCommand1199( objID, memID ); }
		static inline unsigned int CreateObjectFromMeshMemblock( unsigned int memID ) { return AGKCommand1200( memID ); }
		static inline void SetObjectMeshFromMemblock( unsigned int objID, unsigned int meshIndex, unsigned int memID ) { AGKCommand1201( objID, meshIndex, memID ); }
		static inline void AddObjectMeshFromMemblock( unsigned int objID, unsigned int memID ) { AGKCommand1202( objID, memID ); }
		static inline void SetMeshMemblockVertexPosition( unsigned int memID, unsigned int vertexIndex, float x, float y, float z ) { AGKCommand1203( memID, vertexIndex, x, y, z ); }
		static inline void SetMeshMemblockVertexNormal( unsigned int memID, unsigned int vertexIndex, float x, float y, float z ) { AGKCommand1204( memID, vertexIndex, x, y, z ); }
		static inline void SetMeshMemblockVertexUV( unsigned int memID, unsigned int vertexIndex, float u, float v ) { AGKCommand1205( memID, vertexIndex, u, v ); }
		static inline float GetMeshMemblockVertexX( unsigned int memID, unsigned int vertexIndex ) { return AGKCommand1206( memID, vertexIndex ); }
		static inline float GetMeshMemblockVertexY( unsigned int memID, unsigned int vertexIndex ) { return AGKCommand1207( memID, vertexIndex ); }
		static inline float GetMeshMemblockVertexZ( unsigned int memID, unsigned int vertexIndex ) { return AGKCommand1208( memID, vertexIndex ); }
		static inline float GetMeshMemblockVertexNormalX( unsigned int memID, unsigned int vertexIndex ) { return AGKCommand1209( memID, vertexIndex ); }
		static inline float GetMeshMemblockVertexNormalY( unsigned int memID, unsigned int vertexIndex ) { return AGKCommand1210( memID, vertexIndex ); }
		static inline float GetMeshMemblockVertexNormalZ( unsigned int memID, unsigned int vertexIndex ) { return AGKCommand1211( memID, vertexIndex ); }
		static inline float GetMeshMemblockVertexU( unsigned int memID, unsigned int vertexIndex ) { return AGKCommand1212( memID, vertexIndex ); }
		static inline float GetMeshMemblockVertexV( unsigned int memID, unsigned int vertexIndex ) { return AGKCommand1213( memID, vertexIndex ); }
		static inline void SetGlobal3DDepth( int depth ) { AGKCommand1214( depth ); }
		static inline void CreateObjectBox( unsigned int objID, float width, float height, float length ) { AGKCommand1215( objID, width, height, length ); }
		static inline unsigned int CreateObjectBox( float width, float height, float length ) { return AGKCommand1216( width, height, length ); }
		static inline void CreateObjectCapsule( unsigned int objID, float diameter, float height, int axis ) { AGKCommand1217( objID, diameter, height, axis ); }
		static inline unsigned int CreateObjectCapsule( float diameter, float height, int axis ) { return AGKCommand1218( diameter, height, axis ); }
		static inline void CreateObjectSphere( unsigned int objID, float diameter, int rows, int columns ) { AGKCommand1219( objID, diameter, rows, columns ); }
		static inline unsigned int CreateObjectSphere( float diameter, int rows, int columns ) { return AGKCommand1220( diameter, rows, columns ); }
		static inline void CreateObjectCone( unsigned int objID, float height, float diameter, int segments ) { AGKCommand1221( objID, height, diameter, segments ); }
		static inline unsigned int CreateObjectCone( float height, float diameter, int segments ) { return AGKCommand1222( height, diameter, segments ); }
		static inline void CreateObjectCylinder( unsigned int objID, float height, float diameter, int segments ) { AGKCommand1223( objID, height, diameter, segments ); }
		static inline unsigned int CreateObjectCylinder( float height, float diameter, int segments ) { return AGKCommand1224( height, diameter, segments ); }
		static inline void CreateObjectPlane( unsigned int objID, float width, float height ) { AGKCommand1225( objID, width, height ); }
		static inline unsigned int CreateObjectPlane( float width, float height ) { return AGKCommand1226( width, height ); }
		static inline void CreateObjectQuad( unsigned int objID ) { AGKCommand1227( objID ); }
		static inline unsigned int CreateObjectQuad(  ) { return AGKCommand1228(  ); }
		static inline unsigned int CreateObjectFromHeightMap( const char* szImageFile, float width, float height, float length, int smoothing, int split ) { return AGKCommand1229( szImageFile, width, height, length, smoothing, split ); }
		static inline void CreateObjectFromHeightMap( unsigned int objID, const char* szImageFile, float width, float height, float length, int smoothing, int split ) { AGKCommand1230( objID, szImageFile, width, height, length, smoothing, split ); }
		static inline unsigned int CreateObjectFromObjectMesh( unsigned int fromObjID, unsigned int meshIndex ) { return AGKCommand1231( fromObjID, meshIndex ); }
		static inline void CreateObjectFromObjectMesh( unsigned int objID, unsigned int fromObjID, unsigned int meshIndex ) { AGKCommand1232( objID, fromObjID, meshIndex ); }
		static inline unsigned int LoadObject( const char* szFilename ) { return AGKCommand1233( szFilename ); }
		static inline unsigned int LoadObject( const char* szFilename, float height ) { return AGKCommand1234( szFilename, height ); }
		static inline void LoadObject( unsigned int objID, const char* szFilename ) { AGKCommand1235( objID, szFilename ); }
		static inline void LoadObject( unsigned int objID, const char* szFilename, float height ) { AGKCommand1236( objID, szFilename, height ); }
		static inline unsigned int LoadObjectWithChildren( const char* szFilename ) { return AGKCommand1237( szFilename ); }
		static inline void LoadObjectWithChildren( unsigned int objID, const char* szFilename ) { AGKCommand1238( objID, szFilename ); }
		static inline void SaveObject( unsigned int objID, const char * szFilename ) { AGKCommand1239( objID, szFilename ); }
		static inline unsigned int CloneObject( unsigned int objID ) { return AGKCommand1240( objID ); }
		static inline void CloneObject( unsigned int newobjID, unsigned int objID ) { AGKCommand1241( newobjID, objID ); }
		static inline unsigned int InstanceObject( unsigned int objID ) { return AGKCommand1242( objID ); }
		static inline void InstanceObject( unsigned int newobjID, unsigned int objID ) { AGKCommand1243( newobjID, objID ); }
		static inline int GetObjectExists( unsigned int objID ) { return AGKCommand1244( objID ); }
		static inline void DeleteObject( unsigned int objID ) { AGKCommand1245( objID ); }
		static inline void DeleteObjectWithChildren( unsigned int objID ) { AGKCommand1246( objID ); }
		static inline void DeleteAllObjects(  ) { AGKCommand1247(  ); }
		static inline unsigned int GetObjectNumChildren( unsigned int objID ) { return AGKCommand1248( objID ); }
		static inline unsigned int GetObjectChildID( unsigned int objID, int childIndex ) { return AGKCommand1249( objID, childIndex ); }
		static inline unsigned int GetObjectNumBones( unsigned int objID ) { return AGKCommand1250( objID ); }
		static inline unsigned int GetObjectBoneByName( unsigned int objID, const char * name ) { return AGKCommand1251( objID, name ); }
		static inline unsigned int GetObjectNumMeshes( unsigned int objID ) { return AGKCommand1252( objID ); }
		static inline char* GetObjectMeshName( unsigned int objID, unsigned int meshIndex ) { return AGKCommand1253( objID, meshIndex ); }
		static inline void SetObjectMeshImage( unsigned int objID, unsigned int meshIndex, unsigned int imageID, unsigned int textureStage ) { AGKCommand1254( objID, meshIndex, imageID, textureStage ); }
		static inline void SetObjectMeshLightMap( unsigned int objID, unsigned int meshIndex, unsigned int imageID ) { AGKCommand1255( objID, meshIndex, imageID ); }
		static inline void SetObjectMeshShader( unsigned int objID, unsigned int meshIndex, unsigned int shaderID ) { AGKCommand1256( objID, meshIndex, shaderID ); }
		static inline char* GetObjectMeshVSSource( unsigned int objID, unsigned int meshIndex ) { return AGKCommand1257( objID, meshIndex ); }
		static inline char* GetObjectMeshPSSource( unsigned int objID, unsigned int meshIndex ) { return AGKCommand1258( objID, meshIndex ); }
		static inline void SetObjectMeshUVOffset( unsigned int objID, unsigned int meshIndex, unsigned int textureStage, float offsetU, float offsetV ) { AGKCommand1259( objID, meshIndex, textureStage, offsetU, offsetV ); }
		static inline void SetObjectMeshUVScale( unsigned int objID, unsigned int meshIndex, unsigned int textureStage, float scaleU, float scaleV ) { AGKCommand1260( objID, meshIndex, textureStage, scaleU, scaleV ); }
		static inline float GetObjectMeshSizeMinX( unsigned int objID, unsigned int meshIndex ) { return AGKCommand1261( objID, meshIndex ); }
		static inline float GetObjectMeshSizeMaxX( unsigned int objID, unsigned int meshIndex ) { return AGKCommand1262( objID, meshIndex ); }
		static inline float GetObjectMeshSizeMinY( unsigned int objID, unsigned int meshIndex ) { return AGKCommand1263( objID, meshIndex ); }
		static inline float GetObjectMeshSizeMaxY( unsigned int objID, unsigned int meshIndex ) { return AGKCommand1264( objID, meshIndex ); }
		static inline float GetObjectMeshSizeMinZ( unsigned int objID, unsigned int meshIndex ) { return AGKCommand1265( objID, meshIndex ); }
		static inline float GetObjectMeshSizeMaxZ( unsigned int objID, unsigned int meshIndex ) { return AGKCommand1266( objID, meshIndex ); }
		static inline void FixObjectToObject( unsigned int objID, unsigned int toObjID ) { AGKCommand1267( objID, toObjID ); }
		static inline void FixObjectToBone( unsigned int objID, unsigned int toObjID, unsigned int toBoneIndex ) { AGKCommand1268( objID, toObjID, toBoneIndex ); }
		static inline int GetObjectNumAnimations( unsigned int objID ) { return AGKCommand1269( objID ); }
		static inline char* GetObjectAnimationName( unsigned int objID, int index ) { return AGKCommand1270( objID, index ); }
		static inline void PlayObjectAnimation( unsigned int objID, const char * animName, float starttime, float endtime, int loop, float tweentime ) { AGKCommand1271( objID, animName, starttime, endtime, loop, tweentime ); }
		static inline void SetObjectAnimationFrame( unsigned int objID, const char * animName, float time, float tweentime ) { AGKCommand1272( objID, animName, time, tweentime ); }
		static inline void StopObjectAnimation( unsigned int objID ) { AGKCommand1273( objID ); }
		static inline void ResetObjectAnimation( unsigned int objID ) { AGKCommand1274( objID ); }
		static inline void SetObjectAnimationSpeed( unsigned int objID, float speed ) { AGKCommand1275( objID, speed ); }
		static inline int GetObjectIsAnimating( unsigned int objID ) { return AGKCommand1276( objID ); }
		static inline int GetObjectIsTweening( unsigned int objID ) { return AGKCommand1277( objID ); }
		static inline float GetObjectAnimationTime( unsigned int objID ) { return AGKCommand1278( objID ); }
		static inline float GetObjectAnimationDuration( unsigned int objID, const char * animName ) { return AGKCommand1279( objID, animName ); }
		static inline void SetObjectBonePosition( unsigned int objID, unsigned int boneIndex, float x, float y, float z ) { AGKCommand1280( objID, boneIndex, x, y, z ); }
		static inline void SetObjectBoneRotation( unsigned int objID, unsigned int boneIndex, float angx, float angy, float angz ) { AGKCommand1281( objID, boneIndex, angx, angy, angz ); }
		static inline void SetObjectBoneRotationQuat( unsigned int objID, unsigned int boneIndex, float w, float x, float y, float z ) { AGKCommand1282( objID, boneIndex, w, x, y, z ); }
		static inline void SetObjectBoneLookAt( unsigned int objID, unsigned int boneIndex, float x, float y, float z, float roll ) { AGKCommand1283( objID, boneIndex, x, y, z, roll ); }
		static inline void SetObjectBoneCanAnimate( unsigned int objID, unsigned int boneIndex, int animate ) { AGKCommand1284( objID, boneIndex, animate ); }
		static inline void RotateObjectBoneLocalX( unsigned int objID, unsigned int boneIndex, float amount ) { AGKCommand1285( objID, boneIndex, amount ); }
		static inline void RotateObjectBoneLocalY( unsigned int objID, unsigned int boneIndex, float amount ) { AGKCommand1286( objID, boneIndex, amount ); }
		static inline void RotateObjectBoneLocalZ( unsigned int objID, unsigned int boneIndex, float amount ) { AGKCommand1287( objID, boneIndex, amount ); }
		static inline char* GetObjectBoneName( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1288( objID, boneIndex ); }
		static inline float GetObjectBoneX( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1289( objID, boneIndex ); }
		static inline float GetObjectBoneY( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1290( objID, boneIndex ); }
		static inline float GetObjectBoneZ( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1291( objID, boneIndex ); }
		static inline float GetObjectBoneAngleX( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1292( objID, boneIndex ); }
		static inline float GetObjectBoneAngleY( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1293( objID, boneIndex ); }
		static inline float GetObjectBoneAngleZ( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1294( objID, boneIndex ); }
		static inline float GetObjectBoneQuatW( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1295( objID, boneIndex ); }
		static inline float GetObjectBoneQuatX( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1296( objID, boneIndex ); }
		static inline float GetObjectBoneQuatY( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1297( objID, boneIndex ); }
		static inline float GetObjectBoneQuatZ( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1298( objID, boneIndex ); }
		static inline float GetObjectBoneWorldX( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1299( objID, boneIndex ); }
		static inline float GetObjectBoneWorldY( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1300( objID, boneIndex ); }
		static inline float GetObjectBoneWorldZ( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1301( objID, boneIndex ); }
		static inline float GetObjectBoneWorldAngleX( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1302( objID, boneIndex ); }
		static inline float GetObjectBoneWorldAngleY( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1303( objID, boneIndex ); }
		static inline float GetObjectBoneWorldAngleZ( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1304( objID, boneIndex ); }
		static inline float GetObjectBoneWorldQuatW( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1305( objID, boneIndex ); }
		static inline float GetObjectBoneWorldQuatX( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1306( objID, boneIndex ); }
		static inline float GetObjectBoneWorldQuatY( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1307( objID, boneIndex ); }
		static inline float GetObjectBoneWorldQuatZ( unsigned int objID, unsigned int boneIndex ) { return AGKCommand1308( objID, boneIndex ); }
		static inline void SetObjectPosition( unsigned int objID, float x, float y, float z ) { AGKCommand1309( objID, x, y, z ); }
		static inline void SetObjectRotation( unsigned int objID, float angx, float angy, float angz ) { AGKCommand1310( objID, angx, angy, angz ); }
		static inline void SetObjectRotationQuat( unsigned int objID, float w, float x, float y, float z ) { AGKCommand1311( objID, w, x, y, z ); }
		static inline void SetObjectScale( unsigned int objID, float x, float y, float z ) { AGKCommand1312( objID, x, y, z ); }
		static inline void SetObjectScalePermanent( unsigned int objID, float x, float y, float z ) { AGKCommand1313( objID, x, y, z ); }
		static inline void MoveObjectLocalX( unsigned int objID, float amount ) { AGKCommand1314( objID, amount ); }
		static inline void MoveObjectLocalY( unsigned int objID, float amount ) { AGKCommand1315( objID, amount ); }
		static inline void MoveObjectLocalZ( unsigned int objID, float amount ) { AGKCommand1316( objID, amount ); }
		static inline void RotateObjectLocalX( unsigned int objID, float amount ) { AGKCommand1317( objID, amount ); }
		static inline void RotateObjectLocalY( unsigned int objID, float amount ) { AGKCommand1318( objID, amount ); }
		static inline void RotateObjectLocalZ( unsigned int objID, float amount ) { AGKCommand1319( objID, amount ); }
		static inline void RotateObjectGlobalX( unsigned int objID, float amount ) { AGKCommand1320( objID, amount ); }
		static inline void RotateObjectGlobalY( unsigned int objID, float amount ) { AGKCommand1321( objID, amount ); }
		static inline void RotateObjectGlobalZ( unsigned int objID, float amount ) { AGKCommand1322( objID, amount ); }
		static inline float GetObjectX( unsigned int objID ) { return AGKCommand1323( objID ); }
		static inline float GetObjectY( unsigned int objID ) { return AGKCommand1324( objID ); }
		static inline float GetObjectZ( unsigned int objID ) { return AGKCommand1325( objID ); }
		static inline float GetObjectAngleX( unsigned int objID ) { return AGKCommand1326( objID ); }
		static inline float GetObjectAngleY( unsigned int objID ) { return AGKCommand1327( objID ); }
		static inline float GetObjectAngleZ( unsigned int objID ) { return AGKCommand1328( objID ); }
		static inline float GetObjectQuatW( unsigned int objID ) { return AGKCommand1329( objID ); }
		static inline float GetObjectQuatX( unsigned int objID ) { return AGKCommand1330( objID ); }
		static inline float GetObjectQuatY( unsigned int objID ) { return AGKCommand1331( objID ); }
		static inline float GetObjectQuatZ( unsigned int objID ) { return AGKCommand1332( objID ); }
		static inline float GetObjectWorldX( unsigned int objID ) { return AGKCommand1333( objID ); }
		static inline float GetObjectWorldY( unsigned int objID ) { return AGKCommand1334( objID ); }
		static inline float GetObjectWorldZ( unsigned int objID ) { return AGKCommand1335( objID ); }
		static inline float GetObjectWorldAngleX( unsigned int objID ) { return AGKCommand1336( objID ); }
		static inline float GetObjectWorldAngleY( unsigned int objID ) { return AGKCommand1337( objID ); }
		static inline float GetObjectWorldAngleZ( unsigned int objID ) { return AGKCommand1338( objID ); }
		static inline float GetObjectWorldQuatW( unsigned int objID ) { return AGKCommand1339( objID ); }
		static inline float GetObjectWorldQuatX( unsigned int objID ) { return AGKCommand1340( objID ); }
		static inline float GetObjectWorldQuatY( unsigned int objID ) { return AGKCommand1341( objID ); }
		static inline float GetObjectWorldQuatZ( unsigned int objID ) { return AGKCommand1342( objID ); }
		static inline float GetObjectSizeMinX( unsigned int objID ) { return AGKCommand1343( objID ); }
		static inline float GetObjectSizeMaxX( unsigned int objID ) { return AGKCommand1344( objID ); }
		static inline float GetObjectSizeMinY( unsigned int objID ) { return AGKCommand1345( objID ); }
		static inline float GetObjectSizeMaxY( unsigned int objID ) { return AGKCommand1346( objID ); }
		static inline float GetObjectSizeMinZ( unsigned int objID ) { return AGKCommand1347( objID ); }
		static inline float GetObjectSizeMaxZ( unsigned int objID ) { return AGKCommand1348( objID ); }
		static inline void SetObjectLookAt( unsigned int objID, float x, float y, float z, float roll ) { AGKCommand1349( objID, x, y, z, roll ); }
		static inline void FixObjectPivot( unsigned int objID ) { AGKCommand1350( objID ); }
		static inline float GetObjectHeightMapHeight( unsigned int objID, float x, float z ) { return AGKCommand1351( objID, x, z ); }
		static inline void SetObjectImage( unsigned int objID, unsigned int imageID, unsigned int texStage ) { AGKCommand1352( objID, imageID, texStage ); }
		static inline void SetObjectLightMap( unsigned int objID, unsigned int imageID ) { AGKCommand1353( objID, imageID ); }
		static inline void SetObjectShader( unsigned int objID, unsigned int shaderID ) { AGKCommand1354( objID, shaderID ); }
		static inline void SetObjectColor( unsigned int objID, int red, int green, int blue, int alpha ) { AGKCommand1355( objID, red, green, blue, alpha ); }
		static inline void SetObjectColorEmissive( unsigned int objID, int red, int green, int blue ) { AGKCommand1356( objID, red, green, blue ); }
		static inline void SetObjectLightMode( unsigned int objID, int mode ) { AGKCommand1357( objID, mode ); }
		static inline void SetObjectScreenCulling( unsigned int objID, int mode ) { AGKCommand1358( objID, mode ); }
		static inline void SetObjectUVOffset( unsigned int objID, unsigned int textureStage, float offsetU, float offsetV ) { AGKCommand1359( objID, textureStage, offsetU, offsetV ); }
		static inline void SetObjectUVScale( unsigned int objID, unsigned int textureStage, float scaleU, float scaleV ) { AGKCommand1360( objID, textureStage, scaleU, scaleV ); }
		static inline void SetObjectFogMode( unsigned int objID, int mode ) { AGKCommand1361( objID, mode ); }
		static inline void SetObjectDepthReadMode( unsigned int objID, int mode ) { AGKCommand1362( objID, mode ); }
		static inline void SetObjectDepthWrite( unsigned int objID, int mode ) { AGKCommand1363( objID, mode ); }
		static inline void SetObjectDepthBias( unsigned int objID, float bias ) { AGKCommand1364( objID, bias ); }
		static inline void SetObjectDepthRange( unsigned int objID, float zNear, float zFar ) { AGKCommand1365( objID, zNear, zFar ); }
		static inline void SetObjectTransparency( unsigned int objID, int mode ) { AGKCommand1366( objID, mode ); }
		static inline void SetObjectAlphaMask( unsigned int objID, int mode ) { AGKCommand1367( objID, mode ); }
		static inline void SetObjectCullMode( unsigned int objID, int mode ) { AGKCommand1368( objID, mode ); }
		static inline void SetObjectVisible( unsigned int objID, int mode ) { AGKCommand1369( objID, mode ); }
		static inline int GetObjectDepthReadMode( unsigned int objID ) { return AGKCommand1370( objID ); }
		static inline int GetObjectDepthWrite( unsigned int objID ) { return AGKCommand1371( objID ); }
		static inline float GetObjectDepthBias( unsigned int objID ) { return AGKCommand1372( objID ); }
		static inline int GetObjectTransparency( unsigned int objID ) { return AGKCommand1373( objID ); }
		static inline int GetObjectCullMode( unsigned int objID ) { return AGKCommand1374( objID ); }
		static inline int GetObjectVisible( unsigned int objID ) { return AGKCommand1375( objID ); }
		static inline int GetObjectInScreen( unsigned int objID ) { return AGKCommand1376( objID ); }
		static inline char* GetObjectName( unsigned int objID ) { return AGKCommand1377( objID ); }
		static inline void SetObjectShaderConstantByName( unsigned int objID, const char * szName, float value1, float value2, float value3, float value4 ) { AGKCommand1378( objID, szName, value1, value2, value3, value4 ); }
		static inline void SetObjectShaderConstantArrayByName( unsigned int objID, const char * szName, unsigned int arrayIndex, float value1, float value2, float value3, float value4 ) { AGKCommand1379( objID, szName, arrayIndex, value1, value2, value3, value4 ); }
		static inline void SetObjectShaderConstantDefault( unsigned int objID, const char * szName ) { AGKCommand1380( objID, szName ); }
		static inline void DrawObject( unsigned int objID ) { AGKCommand1381( objID ); }
		static inline float GetScreenXFrom3D( float x, float y, float z ) { return AGKCommand1382( x, y, z ); }
		static inline float GetScreenYFrom3D( float x, float y, float z ) { return AGKCommand1383( x, y, z ); }
		static inline float Get3DVectorXFromScreen( float x, float y ) { return AGKCommand1384( x, y ); }
		static inline float Get3DVectorYFromScreen( float x, float y ) { return AGKCommand1385( x, y ); }
		static inline float Get3DVectorZFromScreen( float x, float y ) { return AGKCommand1386( x, y ); }
		static inline void SetObjectCollisionMode( unsigned int objID, int mode ) { AGKCommand1387( objID, mode ); }
		static inline int ObjectRayCast( unsigned int objID, float oldx, float oldy, float oldz, float newx, float newy, float newz ) { return AGKCommand1388( objID, oldx, oldy, oldz, newx, newy, newz ); }
		static inline int ObjectSphereCast( unsigned int objID, float oldx, float oldy, float oldz, float newx, float newy, float newz, float radius ) { return AGKCommand1389( objID, oldx, oldy, oldz, newx, newy, newz, radius ); }
		static inline int ObjectSphereSlide( unsigned int objID, float oldx, float oldy, float oldz, float newx, float newy, float newz, float radius ) { return AGKCommand1390( objID, oldx, oldy, oldz, newx, newy, newz, radius ); }
		static inline unsigned int GetObjectRayCastNumHits(  ) { return AGKCommand1391(  ); }
		static inline unsigned int GetObjectRayCastHitID( unsigned int index ) { return AGKCommand1392( index ); }
		static inline float GetObjectRayCastX( unsigned int index ) { return AGKCommand1393( index ); }
		static inline float GetObjectRayCastY( unsigned int index ) { return AGKCommand1394( index ); }
		static inline float GetObjectRayCastZ( unsigned int index ) { return AGKCommand1395( index ); }
		static inline float GetObjectRayCastSlideX( unsigned int index ) { return AGKCommand1396( index ); }
		static inline float GetObjectRayCastSlideY( unsigned int index ) { return AGKCommand1397( index ); }
		static inline float GetObjectRayCastSlideZ( unsigned int index ) { return AGKCommand1398( index ); }
		static inline float GetObjectRayCastNormalX( unsigned int index ) { return AGKCommand1399( index ); }
		static inline float GetObjectRayCastNormalY( unsigned int index ) { return AGKCommand1400( index ); }
		static inline float GetObjectRayCastNormalZ( unsigned int index ) { return AGKCommand1401( index ); }
		static inline float GetObjectRayCastBounceX( unsigned int index ) { return AGKCommand1402( index ); }
		static inline float GetObjectRayCastBounceY( unsigned int index ) { return AGKCommand1403( index ); }
		static inline float GetObjectRayCastBounceZ( unsigned int index ) { return AGKCommand1404( index ); }
		static inline float GetObjectRayCastDistance( unsigned int index ) { return AGKCommand1405( index ); }
		static inline void SetFogMode( int mode ) { AGKCommand1406( mode ); }
		static inline void SetFogColor( int red, int green, int blue ) { AGKCommand1407( red, green, blue ); }
		static inline void SetFogSunColor( int red, int green, int blue ) { AGKCommand1408( red, green, blue ); }
		static inline void SetFogRange( float minDist, float maxDist ) { AGKCommand1409( minDist, maxDist ); }
		static inline int GetFogMode(  ) { return AGKCommand1410(  ); }
		static inline void SetSkyBoxVisible( int active ) { AGKCommand1411( active ); }
		static inline void SetSkyBoxSkyColor( int red, int green, int blue ) { AGKCommand1412( red, green, blue ); }
		static inline void SetSkyBoxHorizonColor( int red, int green, int blue ) { AGKCommand1413( red, green, blue ); }
		static inline void SetSkyBoxSunColor( int red, int green, int blue ) { AGKCommand1414( red, green, blue ); }
		static inline void SetSkyBoxHorizonSize( float size, float height ) { AGKCommand1415( size, height ); }
		static inline void SetSkyBoxSunVisible( int visible ) { AGKCommand1416( visible ); }
		static inline void SetSkyBoxSunSize( float sun, float halo ) { AGKCommand1417( sun, halo ); }
		static inline unsigned int LoadShader( const char* szVertexFile, const char* szPixelFile ) { return AGKCommand1418( szVertexFile, szPixelFile ); }
		static inline void LoadShader( unsigned int shaderID, const char* szVertexFile, const char* szPixelFile ) { AGKCommand1419( shaderID, szVertexFile, szPixelFile ); }
		static inline unsigned int LoadFullScreenShader( const char* szPixelFile ) { return AGKCommand1420( szPixelFile ); }
		static inline void LoadFullScreenShader( unsigned int shaderID, const char* szPixelFile ) { AGKCommand1421( shaderID, szPixelFile ); }
		static inline unsigned int LoadSpriteShader( const char* szPixelFile ) { return AGKCommand1422( szPixelFile ); }
		static inline void LoadSpriteShader( unsigned int shaderID, const char* szPixelFile ) { AGKCommand1423( shaderID, szPixelFile ); }
		static inline void SetShaderConstantByName( unsigned int shaderID, const char * szName, float value1, float value2, float value3, float value4 ) { AGKCommand1424( shaderID, szName, value1, value2, value3, value4 ); }
		static inline void SetShaderConstantArrayByName( unsigned int shaderID, const char * szName, unsigned int arrayIndex, float value1, float value2, float value3, float value4 ) { AGKCommand1425( shaderID, szName, arrayIndex, value1, value2, value3, value4 ); }
		static inline void SetCameraPosition( unsigned int cameraID, float x, float y, float z ) { AGKCommand1426( cameraID, x, y, z ); }
		static inline void SetCameraRotationQuat( unsigned int cameraID, float w, float x, float y, float z ) { AGKCommand1427( cameraID, w, x, y, z ); }
		static inline void SetCameraRotation( unsigned int cameraID, float angx, float angy, float angz ) { AGKCommand1428( cameraID, angx, angy, angz ); }
		static inline void MoveCameraLocalX( unsigned int cameraID, float amount ) { AGKCommand1429( cameraID, amount ); }
		static inline void MoveCameraLocalY( unsigned int cameraID, float amount ) { AGKCommand1430( cameraID, amount ); }
		static inline void MoveCameraLocalZ( unsigned int cameraID, float amount ) { AGKCommand1431( cameraID, amount ); }
		static inline void RotateCameraLocalX( unsigned int cameraID, float amount ) { AGKCommand1432( cameraID, amount ); }
		static inline void RotateCameraLocalY( unsigned int cameraID, float amount ) { AGKCommand1433( cameraID, amount ); }
		static inline void RotateCameraLocalZ( unsigned int cameraID, float amount ) { AGKCommand1434( cameraID, amount ); }
		static inline void RotateCameraGlobalX( unsigned int cameraID, float amount ) { AGKCommand1435( cameraID, amount ); }
		static inline void RotateCameraGlobalY( unsigned int cameraID, float amount ) { AGKCommand1436( cameraID, amount ); }
		static inline void RotateCameraGlobalZ( unsigned int cameraID, float amount ) { AGKCommand1437( cameraID, amount ); }
		static inline float GetCameraX( unsigned int cameraID ) { return AGKCommand1438( cameraID ); }
		static inline float GetCameraY( unsigned int cameraID ) { return AGKCommand1439( cameraID ); }
		static inline float GetCameraZ( unsigned int cameraID ) { return AGKCommand1440( cameraID ); }
		static inline float GetCameraAngleX( unsigned int cameraID ) { return AGKCommand1441( cameraID ); }
		static inline float GetCameraAngleY( unsigned int cameraID ) { return AGKCommand1442( cameraID ); }
		static inline float GetCameraAngleZ( unsigned int cameraID ) { return AGKCommand1443( cameraID ); }
		static inline float GetCameraQuatW( unsigned int cameraID ) { return AGKCommand1444( cameraID ); }
		static inline float GetCameraQuatX( unsigned int cameraID ) { return AGKCommand1445( cameraID ); }
		static inline float GetCameraQuatY( unsigned int cameraID ) { return AGKCommand1446( cameraID ); }
		static inline float GetCameraQuatZ( unsigned int cameraID ) { return AGKCommand1447( cameraID ); }
		static inline void SetCameraLookAt( unsigned int cameraID, float x, float y, float z, float roll ) { AGKCommand1448( cameraID, x, y, z, roll ); }
		static inline void SetCameraRange( unsigned int cameraID, float fNear, float fFar ) { AGKCommand1449( cameraID, fNear, fFar ); }
		static inline void SetCameraAspect( unsigned int cameraID, float aspect ) { AGKCommand1450( cameraID, aspect ); }
		static inline void SetCameraFOV( unsigned int cameraID, float fov ) { AGKCommand1451( cameraID, fov ); }
		static inline void SetCameraOrthoWidth( unsigned int cameraID, float width ) { AGKCommand1452( cameraID, width ); }
		static inline void SetCameraBounds( unsigned int cameraID, float left, float right, float top, float bottom ) { AGKCommand1453( cameraID, left, right, top, bottom ); }
		static inline void SetCameraOffCenter( unsigned int cameraID, int mode ) { AGKCommand1454( cameraID, mode ); }
		static inline float GetCameraFOV( unsigned int cameraID ) { return AGKCommand1455( cameraID ); }
		static inline void CreatePointLight( unsigned int lightID, float x, float y, float z, float radius, int red, int green, int blue ) { AGKCommand1456( lightID, x, y, z, radius, red, green, blue ); }
		static inline int GetPointLightExists( unsigned int lightID ) { return AGKCommand1457( lightID ); }
		static inline void DeletePointLight( unsigned int lightID ) { AGKCommand1458( lightID ); }
		static inline void ClearPointLights(  ) { AGKCommand1459(  ); }
		static inline void SetPointLightPosition( unsigned int lightID, float x, float y, float z ) { AGKCommand1460( lightID, x, y, z ); }
		static inline void SetPointLightColor( unsigned int lightID, int red, int green, int blue ) { AGKCommand1461( lightID, red, green, blue ); }
		static inline void SetPointLightRadius( unsigned int lightID, float radius ) { AGKCommand1462( lightID, radius ); }
		static inline void SetPointLightMode( unsigned int lightID, int mode ) { AGKCommand1463( lightID, mode ); }
		static inline void SetSunDirection( float vx, float vy, float vz ) { AGKCommand1464( vx, vy, vz ); }
		static inline void SetSunColor( int red, int green, int blue ) { AGKCommand1465( red, green, blue ); }
		static inline void SetSunActive( int active ) { AGKCommand1466( active ); }
		static inline void SetAmbientColor( int red, int green, int blue ) { AGKCommand1467( red, green, blue ); }
		static inline void Create3DPhysicsWorld(  ) { AGKCommand1468(  ); }
		static inline void Create3DPhysicsWorld( float scaleFactor ) { AGKCommand1469( scaleFactor ); }
		static inline void Set3DPhysicsGravity( float x, float y, float z ) { AGKCommand1470( x, y, z ); }
		static inline void Set3DPhysicsGravity( unsigned int vectorID ) { AGKCommand1471( vectorID ); }
		static inline void Step3DPhysicsWorld(  ) { AGKCommand1472(  ); }
		static inline void Reset3DPhysicsWorld(  ) { AGKCommand1473(  ); }
		static inline void Delete3DPhysicsWorld(  ) { AGKCommand1474(  ); }
		static inline void Debug3DPhysicsWorld(  ) { AGKCommand1475(  ); }
		static inline int Get3DPhysicsTotalObjects(  ) { return AGKCommand1476(  ); }
		static inline int Get3DPhysicsActiveObjects(  ) { return AGKCommand1477(  ); }
		static inline int Get3DPhysicsTotalJoints(  ) { return AGKCommand1478(  ); }
		static inline void SetObjectShapeBox( unsigned int objID ) { AGKCommand1479( objID ); }
		static inline void SetObjectShapeBox( unsigned int objID, float sizeX, float sizeY, float sizeZ ) { AGKCommand1480( objID, sizeX, sizeY, sizeZ ); }
		static inline void SetObjectShapeBox( unsigned int objID, unsigned int vectorID ) { AGKCommand1481( objID, vectorID ); }
		static inline void SetObjectShapeSphere( unsigned int objID ) { AGKCommand1482( objID ); }
		static inline void SetObjectShapeSphere( unsigned int objID, float diameter ) { AGKCommand1483( objID, diameter ); }
		static inline void SetObjectShapeCylinder( unsigned int objID, int axis ) { AGKCommand1484( objID, axis ); }
		static inline void SetObjectShapeCylinder( unsigned int objID, int axis, float height, float diameter ) { AGKCommand1485( objID, axis, height, diameter ); }
		static inline void SetObjectShapeCone( unsigned int objID, int axis ) { AGKCommand1486( objID, axis ); }
		static inline void SetObjectShapeCone( unsigned int objID, int axis, float height, float diameter ) { AGKCommand1487( objID, axis, height, diameter ); }
		static inline void SetObjectShapeCapsule( unsigned int objID, int axis ) { AGKCommand1488( objID, axis ); }
		static inline void SetObjectShapeCapsule( unsigned int objID, int axis, float sizeX, float sizeY, float sizeZ ) { AGKCommand1489( objID, axis, sizeX, sizeY, sizeZ ); }
		static inline void SetObjectShapeCapsule( unsigned int objID, int axis, unsigned int vectorID ) { AGKCommand1490( objID, axis, vectorID ); }
		static inline void SetObjectShapeConvexHull( unsigned int objID ) { AGKCommand1491( objID ); }
		static inline void SetObjectShapeStaticPolygon( unsigned int objID ) { AGKCommand1492( objID ); }
		static inline void SetObjectShapeCompound( unsigned int objID ) { AGKCommand1493( objID ); }
		static inline void Create3DPhysicsDynamicBody( unsigned int objID ) { AGKCommand1494( objID ); }
		static inline void Create3DPhysicsStaticBody( unsigned int objID ) { AGKCommand1495( objID ); }
		static inline void Create3DPhysicsKinematicBody( unsigned int objID ) { AGKCommand1496( objID ); }
		static inline void Delete3DPhysicsBody( unsigned int objID ) { AGKCommand1497( objID ); }
		static inline int Create3DPhysicsStaticPlane( float normlX, float normalY, float normalZ, float offsetPosition ) { return AGKCommand1498( normlX, normalY, normalZ, offsetPosition ); }
		static inline void Set3DPhysicsStaticPlanePosition( unsigned int planeID, float posX, float posY, float posZ ) { AGKCommand1499( planeID, posX, posY, posZ ); }
		static inline void Set3DPhysicsStaticPlaneRotation( unsigned int planeID, float angX, float angY, float angZ ) { AGKCommand1500( planeID, angX, angY, angZ ); }
		static inline void Delete3DPhysicsStaticPlane( unsigned int planeID ) { AGKCommand1501( planeID ); }
		static inline void SetObject3DPhysicsGroupAndMask( unsigned int objID, int group, int mask ) { AGKCommand1502( objID, group, mask ); }
		static inline int GetObject3DPhysicsGroup( unsigned int objID ) { return AGKCommand1503( objID ); }
		static inline int GetObject3DPhysicsMask( unsigned int objID ) { return AGKCommand1504( objID ); }
		static inline void SetObject3DPhysicsMass( unsigned int objID, float mass ) { AGKCommand1505( objID, mass ); }
		static inline float GetObject3DPhysicsMass( unsigned int objID ) { return AGKCommand1506( objID ); }
		static inline void SetObject3DPhysicsCanSleep( unsigned int objID, int canSleep ) { AGKCommand1507( objID, canSleep ); }
		static inline void SetObject3DPhysicsFriction( unsigned int objID, float friction ) { AGKCommand1508( objID, friction ); }
		static inline float GetObject3DPhysicsFriction( unsigned int objID ) { return AGKCommand1509( objID ); }
		static inline void SetObject3DPhysicsRollingFriction( unsigned int objID, float friction ) { AGKCommand1510( objID, friction ); }
		static inline float GetObject3DPhysicsRollingFriction( unsigned int objID ) { return AGKCommand1511( objID ); }
		static inline void SetObject3DPhysicsAnisotropicFriction( unsigned int objID, int type ) { AGKCommand1512( objID, type ); }
		static inline void SetObject3DPhysicsMaxLinearVelocity( unsigned int objID, float maxLinearVelocity ) { AGKCommand1513( objID, maxLinearVelocity ); }
		static inline void SetObject3DPhysicsLinearVelocity( unsigned int objID, float dirX, float dirY, float dirZ, float initialSpeed ) { AGKCommand1514( objID, dirX, dirY, dirZ, initialSpeed ); }
		static inline void SetObject3DPhysicsLinearVelocity( unsigned int objID, unsigned int vectorID, float initialSpeed ) { AGKCommand1515( objID, vectorID, initialSpeed ); }
		static inline float GetObject3DPhysicsLinearVelocityX( unsigned int objID ) { return AGKCommand1516( objID ); }
		static inline float GetObject3DPhysicsLinearVelocityY( unsigned int objID ) { return AGKCommand1517( objID ); }
		static inline float GetObject3DPhysicsLinearVelocityZ( unsigned int objID ) { return AGKCommand1518( objID ); }
		static inline void SetObject3DPhysicsAngularVelocity( unsigned int objID, float angX, float angY, float angZ, float initialSpeed ) { AGKCommand1519( objID, angX, angY, angZ, initialSpeed ); }
		static inline void SetObject3DPhysicsAngularVelocity( unsigned int objID, unsigned int vectorID, float initialSpeed ) { AGKCommand1520( objID, vectorID, initialSpeed ); }
		static inline float GetObject3DPhysicsAngularVelocityX( unsigned int objID ) { return AGKCommand1521( objID ); }
		static inline float GetObject3DPhysicsAngularVelocityY( unsigned int objID ) { return AGKCommand1522( objID ); }
		static inline float GetObject3DPhysicsAngularVelocityZ( unsigned int objID ) { return AGKCommand1523( objID ); }
		static inline void SetObject3DPhysicsDamping( unsigned int objID, float linearDamp, float angularDamp ) { AGKCommand1524( objID, linearDamp, angularDamp ); }
		static inline float GetObject3DPhysicsLinearDamp( unsigned int objID ) { return AGKCommand1525( objID ); }
		static inline float GetObject3DPhysicsAngularDamp( unsigned int objID ) { return AGKCommand1526( objID ); }
		static inline void SetObject3DPhysicsSleepingThreshold( unsigned int objID, float angular, float linear ) { AGKCommand1527( objID, angular, linear ); }
		static inline float GetObject3DPhysicsAngularSleepingThreshold( unsigned int objID ) { return AGKCommand1528( objID ); }
		static inline float GetObject3DPhysicsLinearSleepingThreshold( unsigned int objID ) { return AGKCommand1529( objID ); }
		static inline void SetObject3DPhysicsDeactivationTime( unsigned int objID, float time ) { AGKCommand1530( objID, time ); }
		static inline void SetObject3DPhysicsRestitution( unsigned int objID, float friction ) { AGKCommand1531( objID, friction ); }
		static inline float GetObject3DPhysicsRestitution( unsigned int objID ) { return AGKCommand1532( objID ); }
		static inline int GetObject3DPhysicsFirstContact( unsigned int objID ) { return AGKCommand1533( objID ); }
		static inline float GetObject3DPhysicsContactX(  ) { return AGKCommand1534(  ); }
		static inline float GetObject3DPhysicsContactY(  ) { return AGKCommand1535(  ); }
		static inline float GetObject3DPhysicsContactZ(  ) { return AGKCommand1536(  ); }
		static inline int GetObject3DPhysicsContactVector( int outPosVec3 ) { return AGKCommand1537( outPosVec3 ); }
		static inline int GetObject3DPhysicsContactObjectB(  ) { return AGKCommand1538(  ); }
		static inline int GetObject3DPhysicsNextContact(  ) { return AGKCommand1539(  ); }
		static inline int GetObjects3DPhysicsContactPositionVector( unsigned int objA, unsigned int objB, int outPosVec3 ) { return AGKCommand1540( objA, objB, outPosVec3 ); }
		static inline int Create3DPhysicsPickJoint( unsigned int objID, int positionVec3 ) { return AGKCommand1541( objID, positionVec3 ); }
		static inline void Update3DPhysicsPickJoint( unsigned int jointID, int positionVec3 ) { AGKCommand1542( jointID, positionVec3 ); }
		static inline void Delete3DPhysicsPickJoint( unsigned int jointID ) { AGKCommand1543( jointID ); }
		static inline int Create3DPhysicsHingeJoint( unsigned int objA, unsigned int objB, int positionVec3, int rotationVec3, int disableCollisions ) { return AGKCommand1544( objA, objB, positionVec3, rotationVec3, disableCollisions ); }
		static inline int Create3DPhysicsConeTwistJoint( unsigned int objA, unsigned int objB, int positionVec3, int rotationVec3, int disableCollisions ) { return AGKCommand1545( objA, objB, positionVec3, rotationVec3, disableCollisions ); }
		static inline int Create3DPhysicsSliderJoint( unsigned int objA, unsigned int objB, int positionVec3, int rotationVec3 ) { return AGKCommand1546( objA, objB, positionVec3, rotationVec3 ); }
		static inline int Create3DPhysicsFixedJoint( unsigned int objA, unsigned int objB, int positionVec3 ) { return AGKCommand1547( objA, objB, positionVec3 ); }
		static inline int Create3DPhysics6DOFJoint( unsigned int objA, unsigned int objB, int positionVec3, float rotationVec3 ) { return AGKCommand1548( objA, objB, positionVec3, rotationVec3 ); }
		static inline void Set3DPhysicsJointSliderAngularLimits( unsigned int jointID, float lowerLimit, float upperLimit ) { AGKCommand1549( jointID, lowerLimit, upperLimit ); }
		static inline void Set3DPhysicsJointSliderLinearLimits( unsigned int jointID, float lowerLimit, float upperLimit ) { AGKCommand1550( jointID, lowerLimit, upperLimit ); }
		static inline void Set3DPhysicsJointConeTwistLimits( unsigned int jointID, float swingSpan1, float swingSpan2, float twistSpan ) { AGKCommand1551( jointID, swingSpan1, swingSpan2, twistSpan ); }
		static inline void Set3DPhysicsJointHingeLimits( unsigned int jointID, float minAng, float maxAng ) { AGKCommand1552( jointID, minAng, maxAng ); }
		static inline void Set3DPhysicsJointBreakingThreshold( unsigned int jointID, float breakThreshold ) { AGKCommand1553( jointID, breakThreshold ); }
		static inline void Set3DPhysicsJointEnabled( unsigned int jointID, int isEnabled ) { AGKCommand1554( jointID, isEnabled ); }
		static inline int Get3DPhysicsJointEnabled( unsigned int jointID ) { return AGKCommand1555( jointID ); }
		static inline int Get3DPhysicsJointPositionVector( unsigned int jointID ) { return AGKCommand1556( jointID ); }
		static inline int Get3DPhysicsJointRotationVector( unsigned int jointID ) { return AGKCommand1557( jointID ); }
		static inline void Delete3DPhysicsJoint( unsigned int jointID ) { AGKCommand1558( jointID ); }
		static inline void Set3DPhysicsHingeJointMotorIsEnabled( unsigned int jointID, int isEnabled ) { AGKCommand1559( jointID, isEnabled ); }
		static inline void Set3DPhysicsHingeJointMaxMotorImpulse( unsigned int jointID, float maxImpulse ) { AGKCommand1560( jointID, maxImpulse ); }
		static inline void Set3DPhysicsHingeJointMotorVelocity( unsigned int jointID, float targetVelocity ) { AGKCommand1561( jointID, targetVelocity ); }
		static inline void Set3DPhysicsTwistJointMotorIsEnabled( unsigned int jointID, int isEnabled ) { AGKCommand1562( jointID, isEnabled ); }
		static inline void Set3DPhysicsTwistJointMaxMotorImpulse( unsigned int jointID, float maxImpulse ) { AGKCommand1563( jointID, maxImpulse ); }
		static inline void Set3DPhysicsTwistJointMotorRotationTarget( unsigned int jointID, int rotationVec3ID ) { AGKCommand1564( jointID, rotationVec3ID ); }
		static inline void Set3DPhysicsSliderJointPoweredLinearMotorIsEnabled( unsigned int jointID, int isEnabled ) { AGKCommand1565( jointID, isEnabled ); }
		static inline void Set3DPhysicsSliderJointMaxLinearMotorForce( unsigned int jointID, float maxLinearForce ) { AGKCommand1566( jointID, maxLinearForce ); }
		static inline void Set3DPhysicsSliderJointTargetLinearMotorVelocity( unsigned int jointID, float linearMotorVelocity ) { AGKCommand1567( jointID, linearMotorVelocity ); }
		static inline void AddObjectShapeBox( int objID, int positionVec3, int rotationVec3, int sizeVec3 ) { AGKCommand1568( objID, positionVec3, rotationVec3, sizeVec3 ); }
		static inline void AddObjectShapeSphere( int objID, int positionVec3, float diameter ) { AGKCommand1569( objID, positionVec3, diameter ); }
		static inline void AddObjectShapeCapsule( int objID, int positionVec3, int rotationVec3, int sizeVec3, int axis ) { AGKCommand1570( objID, positionVec3, rotationVec3, sizeVec3, axis ); }
		static inline void AddObjectShapeCone( int objID, int positionVec3, int rotationVec3, int sizeVec3, int axis ) { AGKCommand1571( objID, positionVec3, rotationVec3, sizeVec3, axis ); }
		static inline void AddObjectShapeCylinder( int objID, int positionVec3, int rotationVec3, int sizeVec3, int axis ) { AGKCommand1572( objID, positionVec3, rotationVec3, sizeVec3, axis ); }
		static inline int SaveObjectShape( unsigned int objID, const char* fileName ) { return AGKCommand1573( objID, fileName ); }
		static inline int LoadObjectShape( unsigned int objID, const char* fileName ) { return AGKCommand1574( objID, fileName ); }
		static inline int CreateVector3(  ) { return AGKCommand1575(  ); }
		static inline int CreateVector3( float x, float y, float z ) { return AGKCommand1576( x, y, z ); }
		static inline void SetVector3( unsigned int vectorID, float x, float y, float z ) { AGKCommand1577( vectorID, x, y, z ); }
		static inline void DeleteVector3( unsigned int vectorID ) { AGKCommand1578( vectorID ); }
		static inline float GetVector3X( unsigned int vectorID ) { return AGKCommand1579( vectorID ); }
		static inline float GetVector3Y( unsigned int vectorID ) { return AGKCommand1580( vectorID ); }
		static inline float GetVector3Z( unsigned int vectorID ) { return AGKCommand1581( vectorID ); }
		static inline float GetVector3Distance( unsigned int vectorU, unsigned int vectorV ) { return AGKCommand1582( vectorU, vectorV ); }
		static inline float GetVector3Length( unsigned int vectorID ) { return AGKCommand1583( vectorID ); }
		static inline float GetVector3Dot( unsigned int vectorU, unsigned int vectorV ) { return AGKCommand1584( vectorU, vectorV ); }
		static inline void GetVector3Cross( unsigned int resultVec, unsigned int vectorU, unsigned int vectorV ) { AGKCommand1585( resultVec, vectorU, vectorV ); }
		static inline void GetVector3Multiply( unsigned int resultVec, float multiplier ) { AGKCommand1586( resultVec, multiplier ); }
		static inline void GetVector3Add( unsigned int resultVec, unsigned int addVec ) { AGKCommand1587( resultVec, addVec ); }
		static inline int Create3DPhysicsRay(  ) { return AGKCommand1588(  ); }
		static inline void Delete3DPhysicsRay( unsigned int rayID ) { AGKCommand1589( rayID ); }
		static inline int Ray3DPhysicsExist( unsigned int rayID ) { return AGKCommand1590( rayID ); }
		static inline void RayCast3DPhysics( unsigned int rayID, int fromVec3ID, int toVec3ID, int allOrClosest ) { AGKCommand1591( rayID, fromVec3ID, toVec3ID, allOrClosest ); }
		static inline int RayCast3DPhysicsObject( unsigned int objID, unsigned int rayID, int fromVec3ID, int toVec3ID, int allOrClosest ) { return AGKCommand1592( objID, rayID, fromVec3ID, toVec3ID, allOrClosest ); }
		static inline int Get3DPhysicsRayCastObjectHit( unsigned int rayID, float fractionIndex ) { return AGKCommand1593( rayID, fractionIndex ); }
		static inline int Get3DPhysicsRayCastClosestObjectHit( unsigned int rayID ) { return AGKCommand1594( rayID ); }
		static inline float Get3DPhysicsRayCastFraction( unsigned int rayID ) { return AGKCommand1595( rayID ); }
		static inline int Get3DPhysicsRayCastContactPosition( unsigned int rayID, float fractionIndex, int outVec3ID ) { return AGKCommand1596( rayID, fractionIndex, outVec3ID ); }
		static inline int Get3DPhysicsRayCastClosestContactPosition( unsigned int rayID, int outVec3ID ) { return AGKCommand1597( rayID, outVec3ID ); }
		static inline void Get3DPhysicsRayCastNormalVector( unsigned int rayID, int returnVec3ID ) { AGKCommand1598( rayID, returnVec3ID ); }
		static inline int Get3DPhysicsRayCastNumHits( unsigned int rayID ) { return AGKCommand1599( rayID ); }
		static inline void SphereCast3DPhysics( unsigned int rayID, int fromVec3ID, int toVec3ID, float radius ) { AGKCommand1600( rayID, fromVec3ID, toVec3ID, radius ); }
		static inline int SphereCast3DPhysicsObject( unsigned int objID, unsigned int rayID, int fromVec3ID, int toVec3ID, float radius ) { return AGKCommand1601( objID, rayID, fromVec3ID, toVec3ID, radius ); }
		static inline void Create3DPhysicsRagDoll( unsigned int objID, float objTotalWeight ) { AGKCommand1602( objID, objTotalWeight ); }
		static inline int Add3DPhysicsRagDollBone( unsigned int startBoneID, unsigned int endBoneID, float diameter, int collisionGroup, int collisionMask ) { return AGKCommand1603( startBoneID, endBoneID, diameter, collisionGroup, collisionMask ); }
		static inline void AssignTo3DPhysicsRagDollBoneObjectBone( unsigned int ragdollBoneID, unsigned int objBoneID ) { AGKCommand1604( ragdollBoneID, objBoneID ); }
		static inline void Add3DPhysicsRagDollHingeJoint( unsigned int boneAID, unsigned int boneBID, unsigned int objBoneID, int jointRotationVec3, float minLimit, float maxLimit ) { AGKCommand1605( boneAID, boneBID, objBoneID, jointRotationVec3, minLimit, maxLimit ); }
		static inline void Add3DPhysicsRagDollTwistJoint( unsigned int boneAID, unsigned int boneBID, unsigned int objBoneID, int jointRotationVec3, int limitsVec3 ) { AGKCommand1606( boneAID, boneBID, objBoneID, jointRotationVec3, limitsVec3 ); }
		static inline void Finalize3DPhysicsRagDoll(  ) { AGKCommand1607(  ); }
		static inline void Delete3DPhysicsRagdoll( unsigned int objID ) { AGKCommand1608( objID ); }
		static inline int Get3DPhysicsRagdollExist( unsigned int objID ) { return AGKCommand1609( objID ); }
		static inline int Get3DPhysicsRagdollFromBoneObject( unsigned int objID ) { return AGKCommand1610( objID ); }
		static inline int Is3dPhysicsRagdollStatic( unsigned int objID ) { return AGKCommand1611( objID ); }
		static inline void Set3DphysicsRagdollStatic( unsigned int objID, int isStatic ) { AGKCommand1612( objID, isStatic ); }
		static inline void Set3DPhysicsRagdollBonesVisible( unsigned int objID, int isVisible ) { AGKCommand1613( objID, isVisible ); }
		static inline void Set3DPhysicsRagdollDamping( float linear, float angular ) { AGKCommand1614( linear, angular ); }
		static inline void Set3DPhysicsRagdollSleepingThresholds( float linear, float angular ) { AGKCommand1615( linear, angular ); }
		static inline void Set3DPhysicsRagdollDeactivationTime( float time ) { AGKCommand1616( time ); }
		static inline void Set3DPhysicsRagdollDeactivation( unsigned int objID, int isDisabled ) { AGKCommand1617( objID, isDisabled ); }
		static inline void Create3DPhysicsCharacterController( unsigned int objID, int axis, int objOffsetVec3, int objOrientationVec3, float crouchScale ) { AGKCommand1618( objID, axis, objOffsetVec3, objOrientationVec3, crouchScale ); }
		static inline void Delete3DPhysicsCharacterController( unsigned int objID ) { AGKCommand1619( objID ); }
		static inline int Get3DPhysicsCharacterControllerExists( unsigned int objID ) { return AGKCommand1620( objID ); }
		static inline void Set3DPhysicsCharacterControllerGravity( unsigned int objID, float gravity ) { AGKCommand1621( objID, gravity ); }
		static inline void Set3DPhysicsCharacterControllerFallSpeed( unsigned int objID, float fallSpeed ) { AGKCommand1622( objID, fallSpeed ); }
		static inline void Set3DPhysicsCharacterControllerJumpSpeed( unsigned int objID, float jumpSpeed ) { AGKCommand1623( objID, jumpSpeed ); }
		static inline void Set3DPhysicsCharacterControllerMaxSlope( unsigned int objID, float maxSlopeDegress ) { AGKCommand1624( objID, maxSlopeDegress ); }
		static inline void Set3DPhysicsCharacterControllerStepHeight( unsigned int objID, float stepHeight ) { AGKCommand1625( objID, stepHeight ); }
		static inline float Get3DPhysicsCharacterControllerMaxSlope( unsigned int objID ) { return AGKCommand1626( objID ); }
		static inline float Get3DPhysicsCharacterControllerGravity( unsigned int objID ) { return AGKCommand1627( objID ); }
		static inline void Move3DPhysicsCharacterController( unsigned int objID, int direction, float velocity ) { AGKCommand1628( objID, direction, velocity ); }
		static inline void Rotate3DPhysicsCharacterController( unsigned int objID, float angle ) { AGKCommand1629( objID, angle ); }
		static inline void Jump3DPhysicsCharacterController( unsigned int objID ) { AGKCommand1630( objID ); }
		static inline void Crouch3DPhysicsCharacterController( unsigned int objID ) { AGKCommand1631( objID ); }
		static inline void Stand3DPhysicsCharacterController( unsigned int objID ) { AGKCommand1632( objID ); }
		static inline void Debug3DPhysicsCharacterController( unsigned int objID, int isDebug ) { AGKCommand1633( objID, isDebug ); }
		static inline void Set3DPhysicsCharacterControllerPosition( unsigned int objID, float posX, float posY, float posZ ) { AGKCommand1634( objID, posX, posY, posZ ); }
		static inline void SetWindowPosition( int x, int y ) { AGKCommand1635( x, y ); }
		static inline void SetWindowSize( int width, int height, int fullscreen ) { AGKCommand1636( width, height, fullscreen ); }
		static inline void SetWindowSize( int width, int height, int fullscreen, int allowOverSized ) { AGKCommand1637( width, height, fullscreen, allowOverSized ); }
		static inline void SetWindowAllowResize( int mode ) { AGKCommand1638( mode ); }
		static inline void MaximizeWindow(  ) { AGKCommand1639(  ); }
		static inline void MinimizeApp(  ) { AGKCommand1640(  ); }
		static inline void RestoreApp(  ) { AGKCommand1641(  ); }
		static inline void SetImmersiveMode( int mode ) { AGKCommand1642( mode ); }
		static inline void SetScreenResolution( int width, int height ) { AGKCommand1643( width, height ); }
		static inline char* GetDeviceName(  ) { return AGKCommand1644(  ); }
		static inline char* GetDeviceBaseName(  ) { return AGKCommand1645(  ); }
		static inline char* GetDeviceType(  ) { return AGKCommand1646(  ); }
		static inline char* GetAppName(  ) { return AGKCommand1647(  ); }
		static inline char* GetDeviceLanguage(  ) { return AGKCommand1648(  ); }
		static inline void SetSleepMode( int mode ) { AGKCommand1649( mode ); }
		static inline void SetExpansionFileKey( const char * key ) { AGKCommand1650( key ); }
		static inline void SetExpansionFileVersion( int version ) { AGKCommand1651( version ); }
		static inline int GetExpansionFileState(  ) { return AGKCommand1652(  ); }
		static inline void DownloadExpansionFile(  ) { AGKCommand1653(  ); }
		static inline float GetExpansionFileProgress(  ) { return AGKCommand1654(  ); }
		static inline void SetWindowTitle( const char * szTitle ) { AGKCommand1655( szTitle ); }
		static inline void Sleep( unsigned int milliseconds ) { AGKCommand1656( milliseconds ); }
		static inline void SetVSync( int mode ) { AGKCommand1657( mode ); }
		static inline int GetMaxDeviceWidth(  ) { return AGKCommand1658(  ); }
		static inline int GetMaxDeviceHeight(  ) { return AGKCommand1659(  ); }
		static inline int GetDeviceDPI(  ) { return AGKCommand1660(  ); }
		static inline char* GetAppPackageName(  ) { return AGKCommand1661(  ); }
		static inline int GetDevicePlatform(  ) { return AGKCommand1662(  ); }
		static inline float Sin( float a ) { return AGKCommand1663( a ); }
		static inline float Cos( float a ) { return AGKCommand1664( a ); }
		static inline float Tan( float a ) { return AGKCommand1665( a ); }
		static inline float SinRad( float a ) { return AGKCommand1666( a ); }
		static inline float CosRad( float a ) { return AGKCommand1667( a ); }
		static inline float TanRad( float a ) { return AGKCommand1668( a ); }
		static inline float ASin( float a ) { return AGKCommand1669( a ); }
		static inline float ACos( float a ) { return AGKCommand1670( a ); }
		static inline float ATan( float a ) { return AGKCommand1671( a ); }
		static inline float ATan2( float y, float x ) { return AGKCommand1672( y, x ); }
		static inline float ASinRad( float a ) { return AGKCommand1673( a ); }
		static inline float ACosRad( float a ) { return AGKCommand1674( a ); }
		static inline float ATanRad( float a ) { return AGKCommand1675( a ); }
		static inline float ATan2Rad( float y, float x ) { return AGKCommand1676( y, x ); }
		static inline int Trunc( float a ) { return AGKCommand1677( a ); }
		static inline int Floor( float a ) { return AGKCommand1678( a ); }
		static inline int Ceil( float a ) { return AGKCommand1679( a ); }
		static inline int Round( float a ) { return AGKCommand1680( a ); }
		static inline float Sqrt( float a ) { return AGKCommand1681( a ); }
		static inline float Abs( float a ) { return AGKCommand1682( a ); }
		static inline int Mod( int a, int b ) { return AGKCommand1683( a, b ); }
		static inline float FMod( float a, float b ) { return AGKCommand1684( a, b ); }
		static inline float Pow( float a, float b ) { return AGKCommand1685( a, b ); }
		static inline float Log( float a ) { return AGKCommand1686( a ); }
		static inline int GetNumDeviceCameras(  ) { return AGKCommand1687(  ); }
		static inline int SetDeviceCameraToImage( unsigned int cameraID, unsigned int imageID ) { return AGKCommand1688( cameraID, imageID ); }
		static inline int GetDeviceCameraType( unsigned int cameraID ) { return AGKCommand1689( cameraID ); }
		static inline void VibrateDevice( float seconds ) { AGKCommand1690( seconds ); }
		static inline int LoadVideo( const char * szFilename ) { return AGKCommand1691( szFilename ); }
		static inline void DeleteVideo(  ) { AGKCommand1692(  ); }
		static inline void SetVideoDimensions( float x, float y, float width, float height ) { AGKCommand1693( x, y, width, height ); }
		static inline void PlayVideoToImage( unsigned int imageID ) { AGKCommand1694( imageID ); }
		static inline void PlayVideo(  ) { AGKCommand1695(  ); }
		static inline void PauseVideo(  ) { AGKCommand1696(  ); }
		static inline void StopVideo(  ) { AGKCommand1697(  ); }
		static inline int GetVideoPlaying(  ) { return AGKCommand1698(  ); }
		static inline float GetVideoPosition(  ) { return AGKCommand1699(  ); }
		static inline float GetVideoDuration(  ) { return AGKCommand1700(  ); }
		static inline void SetVideoVolume( float volume ) { AGKCommand1701( volume ); }
		static inline float GetVideoWidth(  ) { return AGKCommand1702(  ); }
		static inline float GetVideoHeight(  ) { return AGKCommand1703(  ); }
		static inline void SetVideoPosition( float seconds ) { AGKCommand1704( seconds ); }
		static inline void SetRawWritePath( const char* str ) { AGKCommand1705( str ); }
		static inline char* GetWritePath(  ) { return AGKCommand1706(  ); }
		static inline char* GetReadPath(  ) { return AGKCommand1707(  ); }
		static inline char* GetDocumentsPath(  ) { return AGKCommand1708(  ); }
		static inline int SetCurrentDir( const char* szPath ) { return AGKCommand1709( szPath ); }
		static inline int MakeFolder( const char* szName ) { return AGKCommand1710( szName ); }
		static inline void DeleteFolder( const char* szName ) { AGKCommand1711( szName ); }
		static inline int GetMultiTouchExists(  ) { return AGKCommand1712(  ); }
		static inline int GetMouseExists(  ) { return AGKCommand1713(  ); }
		static inline int GetKeyboardExists(  ) { return AGKCommand1714(  ); }
		static inline int GetCameraExists(  ) { return AGKCommand1715(  ); }
		static inline int GetGPSSensorExists(  ) { return AGKCommand1716(  ); }
		static inline void SetRawMouseVisible( int visible ) { AGKCommand1717( visible ); }
		static inline void SetRawMousePosition( float x, float y ) { AGKCommand1718( x, y ); }
		static inline int GetUnixTime(  ) { return AGKCommand1719(  ); }
		static inline int GetDayOfWeek(  ) { return AGKCommand1720(  ); }
		static inline char* GetCurrentDate(  ) { return AGKCommand1721(  ); }
		static inline char* GetCurrentTime(  ) { return AGKCommand1722(  ); }
		static inline void OpenBrowser( const char * url ) { AGKCommand1723( url ); }
		static inline unsigned int RunApp( const char * szFilename, const char * szParameters ) { return AGKCommand1724( szFilename, szParameters ); }
		static inline unsigned int GetAppRunning( unsigned int appID ) { return AGKCommand1725( appID ); }
		static inline void TerminateApp( unsigned int appID ) { AGKCommand1726( appID ); }
		static inline void ViewFile( const char* szFilename ) { AGKCommand1727( szFilename ); }
		static inline void ShareText( const char* szText ) { AGKCommand1728( szText ); }
		static inline void ShareImage( const char* szFilename ) { AGKCommand1729( szFilename ); }
		static inline void ShareImageAndText( const char* szFilename, const char* szText ) { AGKCommand1730( szFilename, szText ); }
		static inline void FacebookActivateAppTracking(  ) { AGKCommand1731(  ); }
		static inline int GetInternetState(  ) { return AGKCommand1732(  ); }
		static inline void SetPushNotificationKeys( const char* data1, const char* reserved ) { AGKCommand1733( data1, reserved ); }
		static inline int PushNotificationSetup(  ) { return AGKCommand1734(  ); }
		static inline char* GetPushNotificationToken(  ) { return AGKCommand1735(  ); }
		static inline int GetNotificationType(  ) { return AGKCommand1736(  ); }
		static inline void SetNotificationImage( int iImageIndex ) { AGKCommand1737( iImageIndex ); }
		static inline void SetNotificationText( const char* pText ) { AGKCommand1738( pText ); }
		static inline int GetNFCExists(  ) { return AGKCommand1739(  ); }
		static inline void StartGPSTracking(  ) { AGKCommand1740(  ); }
		static inline void StopGPSTracking(  ) { AGKCommand1741(  ); }
		static inline float GetRawGPSLatitude(  ) { return AGKCommand1742(  ); }
		static inline float GetRawGPSLongitude(  ) { return AGKCommand1743(  ); }
		static inline float GetRawGPSAltitude(  ) { return AGKCommand1744(  ); }
		static inline int GetGameCenterExists(  ) { return AGKCommand1745(  ); }
		static inline void GameCenterSetup(  ) { AGKCommand1746(  ); }
		static inline void GameCenterLogin(  ) { AGKCommand1747(  ); }
		static inline int GetGameCenterLoggedIn(  ) { return AGKCommand1748(  ); }
		static inline char* GetGameCenterPlayerID(  ) { return AGKCommand1749(  ); }
		static inline char* GetGameCenterPlayerDisplayName(  ) { return AGKCommand1750(  ); }
		static inline void GameCenterSubmitScore( int iScore, const char* szBoardID ) { AGKCommand1751( iScore, szBoardID ); }
		static inline void GameCenterShowLeaderBoard( const char* szBoardID ) { AGKCommand1752( szBoardID ); }
		static inline void GameCenterSubmitAchievement( const char* szAchievementID, int iPercentageComplete ) { AGKCommand1753( szAchievementID, iPercentageComplete ); }
		static inline void GameCenterAchievementsShow(  ) { AGKCommand1754(  ); }
		static inline void GameCenterAchievementsReset(  ) { AGKCommand1755(  ); }
		static inline void SetSharedVariableAppGroup( const char* group ) { AGKCommand1756( group ); }
		static inline void SaveSharedVariable( const char * varName, const char * varValue ) { AGKCommand1757( varName, varValue ); }
		static inline char* LoadSharedVariable( const char * varName, const char * defaultValue ) { return AGKCommand1758( varName, defaultValue ); }
		static inline void DeleteSharedVariable( const char * varName ) { AGKCommand1759( varName ); }
		static inline void FirebaseSetup(  ) { AGKCommand1760(  ); }
		static inline void FirebaseLogEvent( const char * event_name ) { AGKCommand1761( event_name ); }
		static inline unsigned int IsSupportedDepthTexture(  ) { return AGKCommand1762(  ); }
		static inline void SetAntialiasMode( int mode ) { AGKCommand1763( mode ); }
		static inline void RenderShadowMap(  ) { AGKCommand1764(  ); }
		static inline void SetNetworkAllowClients( unsigned int iNetID ) { AGKCommand1765( iNetID ); }
		static inline void KickNetworkClient( unsigned int iNetID, unsigned int client ) { AGKCommand1766( iNetID, client ); }
		static inline void SetObjectCastShadow( int objID, int mode ) { AGKCommand1767( objID, mode ); }
		static inline void SetObjectReceiveShadow( int objID, int mode ) { AGKCommand1768( objID, mode ); }
		static inline int GetObjectCastShadowMode( int objID ) { return AGKCommand1769( objID ); }
		static inline int GetObjectReceiveShadowMode( int objID ) { return AGKCommand1770( objID ); }
		static inline int GetShadowMappingSupported(  ) { return AGKCommand1771(  ); }
		static inline void SetShadowMappingMode( int mode ) { AGKCommand1772( mode ); }
		static inline int GetShadowMappingMode(  ) { return AGKCommand1773(  ); }
		static inline void SetShadowMapSize( int width, int height ) { AGKCommand1774( width, height ); }
		static inline void SetShadowRange( float range ) { AGKCommand1775( range ); }
		static inline void SetShadowBias( float bias ) { AGKCommand1776( bias ); }
		static inline void SetShadowLightStepSize( float step ) { AGKCommand1777( step ); }
		static inline void SetShadowSmoothing( int mode ) { AGKCommand1778( mode ); }
		static inline unsigned int GetShadowPolygonsDrawn(  ) { return AGKCommand1779(  ); }
		static inline unsigned int LoadShaderFromString( const char* szVertexSource, const char* szPixelSource ) { return AGKCommand1780( szVertexSource, szPixelSource ); }
		static inline void LoadShaderFromString( unsigned int shaderID, const char* szVertexSource, const char* szPixelSource ) { AGKCommand1781( shaderID, szVertexSource, szPixelSource ); }
		static inline void Create3DParticles( unsigned int ID, float x, float y, float z ) { AGKCommand1782( ID, x, y, z ); }
		static inline unsigned int Create3DParticles( float x, float y, float z ) { return AGKCommand1783( x, y, z ); }
		static inline int Get3DParticlesExists( unsigned int ID ) { return AGKCommand1784( ID ); }
		static inline void Delete3DParticles( unsigned int ID ) { AGKCommand1785( ID ); }
		static inline void Set3DParticlesPosition( unsigned int ID, float x, float y, float z ) { AGKCommand1786( ID, x, y, z ); }
		static inline void Set3DParticlesFrequency( unsigned int ID, float freq ) { AGKCommand1787( ID, freq ); }
		static inline void Set3DParticlesStartZone( unsigned int ID, float x1, float y1, float z1, float x2, float y2, float z2 ) { AGKCommand1788( ID, x1, y1, z1, x2, y2, z2 ); }
		static inline void Set3DParticlesDirection( unsigned int ID, float vx, float vy, float vz, float roll ) { AGKCommand1789( ID, vx, vy, vz, roll ); }
		static inline void Set3DParticlesVelocityRange( unsigned int ID, float v1, float v2 ) { AGKCommand1790( ID, v1, v2 ); }
		static inline void Set3DParticlesDirectionRange( unsigned int ID, float angle, float angle2 ) { AGKCommand1791( ID, angle, angle2 ); }
		static inline void Set3DParticlesSize( unsigned int ID, float size ) { AGKCommand1792( ID, size ); }
		static inline void Set3DParticlesLife( unsigned int ID, float time ) { AGKCommand1793( ID, time ); }
		static inline void Set3DParticlesMax( unsigned int ID, int max ) { AGKCommand1794( ID, max ); }
		static inline void Reset3DParticleCount( unsigned int ID ) { AGKCommand1795( ID ); }
		static inline void Set3DParticlesImage( unsigned int ID, unsigned int imageID ) { AGKCommand1796( ID, imageID ); }
		static inline void Set3DParticlesVisible( unsigned int ID, int visible ) { AGKCommand1797( ID, visible ); }
		static inline void Set3DParticlesActive( unsigned int ID, int active ) { AGKCommand1798( ID, active ); }
		static inline void Set3DParticlesTransparency( unsigned int ID, int active ) { AGKCommand1799( ID, active ); }
		static inline void Set3DParticlesColorInterpolation( unsigned int ID, int mode ) { AGKCommand1800( ID, mode ); }
		static inline float Get3DParticlesX( unsigned int ID ) { return AGKCommand1801( ID ); }
		static inline float Get3DParticlesY( unsigned int ID ) { return AGKCommand1802( ID ); }
		static inline float Get3DParticlesZ( unsigned int ID ) { return AGKCommand1803( ID ); }
		static inline int Get3DParticlesVisible( unsigned int ID ) { return AGKCommand1804( ID ); }
		static inline int Get3DParticlesActive( unsigned int ID ) { return AGKCommand1805( ID ); }
		static inline float Get3DParticlesFrequency( unsigned int ID ) { return AGKCommand1806( ID ); }
		static inline float Get3DParticlesDirectionX( unsigned int ID ) { return AGKCommand1807( ID ); }
		static inline float Get3DParticlesDirectionY( unsigned int ID ) { return AGKCommand1808( ID ); }
		static inline float Get3DParticlesDirectionZ( unsigned int ID ) { return AGKCommand1809( ID ); }
		static inline float Get3DParticlesDirectionRange1( unsigned int ID ) { return AGKCommand1810( ID ); }
		static inline float Get3DParticlesDirectionRange2( unsigned int ID ) { return AGKCommand1811( ID ); }
		static inline float Get3DParticlesSize( unsigned int ID ) { return AGKCommand1812( ID ); }
		static inline float Get3DParticlesLife( unsigned int ID ) { return AGKCommand1813( ID ); }
		static inline int Get3DParticlesMaxReached( unsigned int ID ) { return AGKCommand1814( ID ); }
		static inline void Add3DParticlesForce( unsigned int ID, float starttime, float endtime, float x, float y, float z ) { AGKCommand1815( ID, starttime, endtime, x, y, z ); }
		static inline void Clear3DParticlesForces( unsigned int ID ) { AGKCommand1816( ID ); }
		static inline void Add3DParticlesColorKeyFrame( unsigned int ID, float time, unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha ) { AGKCommand1817( ID, time, red, green, blue, alpha ); }
		static inline void Clear3DParticlesColors( unsigned int ID ) { AGKCommand1818( ID ); }
		static inline void Add3DParticlesScaleKeyFrame( unsigned int ID, float time, float scale ) { AGKCommand1819( ID, time, scale ); }
		static inline void Clear3DParticlesScales( unsigned int ID ) { AGKCommand1820( ID ); }
		static inline void Update3DParticles( unsigned int ID, float time ) { AGKCommand1821( ID, time ); }
		static inline void Offset3DParticles( unsigned int ID, float x, float y, float z ) { AGKCommand1822( ID, x, y, z ); }
		static inline void SetMeshMemblockVertexColor( unsigned int memID, unsigned int vertexIndex, int red, int green, int blue, int alpha ) { AGKCommand1823( memID, vertexIndex, red, green, blue, alpha ); }
		static inline int GetMeshMemblockVertexRed( unsigned int memID, unsigned int vertexIndex ) { return AGKCommand1824( memID, vertexIndex ); }
		static inline int GetMeshMemblockVertexGreen( unsigned int memID, unsigned int vertexIndex ) { return AGKCommand1825( memID, vertexIndex ); }
		static inline int GetMeshMemblockVertexBlue( unsigned int memID, unsigned int vertexIndex ) { return AGKCommand1826( memID, vertexIndex ); }
		static inline int GetMeshMemblockVertexAlpha( unsigned int memID, unsigned int vertexIndex ) { return AGKCommand1827( memID, vertexIndex ); }
		static inline void SetObjectMeshNormalMap( unsigned int objID, unsigned int meshIndex, unsigned int imageID ) { AGKCommand1828( objID, meshIndex, imageID ); }
		static inline void SetObjectNormalMap( unsigned int objID, unsigned int imageID ) { AGKCommand1829( objID, imageID ); }
		static inline void SetObjectNormalMapScale( unsigned int objID, float scaleU, float scaleV ) { AGKCommand1830( objID, scaleU, scaleV ); }
		static inline void SetObjectMeshNormalMapScale( unsigned int objID, unsigned int meshIndex, float scaleU, float scaleV ) { AGKCommand1831( objID, meshIndex, scaleU, scaleV ); }
		static inline unsigned char* GetMemblockPtr( unsigned int memID ) { return AGKCommand1832( memID ); }
		static inline void StartScreenRecording( const char * szFilename, int microphone ) { AGKCommand1833( szFilename, microphone ); }
		static inline void StopScreenRecording(  ) { AGKCommand1834(  ); }
		static inline int IsScreenRecording(  ) { return AGKCommand1835(  ); }
		static inline void DeleteShader( unsigned int shaderID ) { AGKCommand1836( shaderID ); }
		static inline void ActivateSmartWatch( const char * szReserved ) { AGKCommand1837( szReserved ); }
		static inline int GetSmartWatchState(  ) { return AGKCommand1838(  ); }
		static inline void SendSmartWatchData( const char* szJson ) { AGKCommand1839( szJson ); }
		static inline char* ReceiveSmartWatchData(  ) { return AGKCommand1840(  ); }
		static inline void TextToSpeechSetup(  ) { AGKCommand1841(  ); }
		static inline int GetTextToSpeechReady(  ) { return AGKCommand1842(  ); }
		static inline void Speak( const char * text ) { AGKCommand1843( text ); }
		static inline void SetSpeechRate( float rate ) { AGKCommand1844( rate ); }
		static inline void SetSpeechLanguage( const char* lang ) { AGKCommand1845( lang ); }
		static inline int IsSpeaking(  ) { return AGKCommand1846(  ); }
		static inline void StopSpeaking(  ) { AGKCommand1847(  ); }
		static inline int GetSpriteTransparency( unsigned int iSpriteIndex ) { return AGKCommand1848( iSpriteIndex ); }
		static inline void SetSpriteShape( unsigned int iSpriteIndex, int shape, int shapeID ) { AGKCommand1849( iSpriteIndex, shape, shapeID ); }
		static inline void SetSpriteShapeBox( unsigned int iSpriteIndex, float x, float y, float x2, float y2, float angle, int shapeID ) { AGKCommand1850( iSpriteIndex, x, y, x2, y2, angle, shapeID ); }
		static inline void SetSpriteShapeCircle( unsigned int iSpriteIndex, float x, float y, float radius, int shapeID ) { AGKCommand1851( iSpriteIndex, x, y, radius, shapeID ); }
		static inline void SetSpriteShapePolygon( unsigned int iSpriteIndex, unsigned int numPoints, unsigned int index, float x, float y, int shapeID ) { AGKCommand1852( iSpriteIndex, numPoints, index, x, y, shapeID ); }
		static inline void SetSpriteShapeChain( unsigned int iSpriteIndex, unsigned int numPoints, unsigned int index, int loop, float x, float y, int shapeID ) { AGKCommand1853( iSpriteIndex, numPoints, index, loop, x, y, shapeID ); }
		static inline void AddSpriteShapeChain( unsigned int iSpriteIndex, unsigned int numPoints, unsigned int index, int loop, float x, float y ) { AGKCommand1854( iSpriteIndex, numPoints, index, loop, x, y ); }
		static inline int GetSpriteNumShapes( unsigned int iSpriteIndex ) { return AGKCommand1855( iSpriteIndex ); }
		static inline int GetSpriteShapeNumVertices( unsigned int iSpriteIndex, int shapeID ) { return AGKCommand1856( iSpriteIndex, shapeID ); }
		static inline float GetSpriteShapeVertexX( unsigned int iSpriteIndex, int shapeID, int vertex ) { return AGKCommand1857( iSpriteIndex, shapeID, vertex ); }
		static inline float GetSpriteShapeVertexY( unsigned int iSpriteIndex, int shapeID, int vertex ) { return AGKCommand1858( iSpriteIndex, shapeID, vertex ); }
		static inline float GetSpritePhysicsCOMX( unsigned int iSpriteIndex ) { return AGKCommand1859( iSpriteIndex ); }
		static inline float GetSpritePhysicsCOMY( unsigned int iSpriteIndex ) { return AGKCommand1860( iSpriteIndex ); }
		static inline void SetSpritePhysicsFriction( unsigned int iSpriteIndex, float friction, int shapeID ) { AGKCommand1861( iSpriteIndex, friction, shapeID ); }
		static inline void SetSpritePhysicsRestitution( unsigned int iSpriteIndex, float restitution, int shapeID ) { AGKCommand1862( iSpriteIndex, restitution, shapeID ); }
		static inline void SetSpritePhysicsDensity( unsigned int iSpriteIndex, float density, int shapeID ) { AGKCommand1863( iSpriteIndex, density, shapeID ); }
		static inline void SetSpritePhysicsIsSensor( unsigned int iSpriteIndex, int sensor, int shapeID ) { AGKCommand1864( iSpriteIndex, sensor, shapeID ); }
		static inline void SetSpriteGroup( unsigned int iSpriteIndex, int group, int shapeID ) { AGKCommand1865( iSpriteIndex, group, shapeID ); }
		static inline void SetSpriteCategoryBits( unsigned int iSpriteIndex, unsigned int categories, int shapeID ) { AGKCommand1866( iSpriteIndex, categories, shapeID ); }
		static inline void SetSpriteCategoryBit( unsigned int iSpriteIndex, unsigned int category, int flag, int shapeID ) { AGKCommand1867( iSpriteIndex, category, flag, shapeID ); }
		static inline void SetSpriteCollideBits( unsigned int iSpriteIndex, unsigned int mask, int shapeID ) { AGKCommand1868( iSpriteIndex, mask, shapeID ); }
		static inline void SetSpriteCollideBit( unsigned int iSpriteIndex, unsigned int category, int flag, int shapeID ) { AGKCommand1869( iSpriteIndex, category, flag, shapeID ); }
		static inline void CreateRopeJoint( unsigned int iJointIndex, unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float x, float y, float x2, float y2, float maxLength, int colConnected ) { AGKCommand1870( iJointIndex, iSpriteIndex1, iSpriteIndex2, x, y, x2, y2, maxLength, colConnected ); }
		static inline unsigned int CreateRopeJoint( unsigned int iSpriteIndex1, unsigned int iSpriteIndex2, float x, float y, float x2, float y2, float maxLength, int colConnected ) { return AGKCommand1871( iSpriteIndex1, iSpriteIndex2, x, y, x2, y2, maxLength, colConnected ); }
		static inline void SetJointMouseMaxForce( unsigned int iJointIndex, float maxForce ) { AGKCommand1872( iJointIndex, maxForce ); }
		static inline void SetJointDamping( unsigned int iJointIndex, float dampingRatio, float frequency ) { AGKCommand1873( iJointIndex, dampingRatio, frequency ); }
		static inline void PauseTweenCustom( unsigned int tweenID ) { AGKCommand1874( tweenID ); }
		static inline void ResumeTweenCustom( unsigned int tweenID ) { AGKCommand1875( tweenID ); }
		static inline void PauseTweenSprite( unsigned int tweenID, unsigned int spriteID ) { AGKCommand1876( tweenID, spriteID ); }
		static inline void ResumeTweenSprite( unsigned int tweenID, unsigned int spriteID ) { AGKCommand1877( tweenID, spriteID ); }
		static inline void PauseTweenText( unsigned int tweenID, unsigned int textID ) { AGKCommand1878( tweenID, textID ); }
		static inline void ResumeTweenText( unsigned int tweenID, unsigned int textID ) { AGKCommand1879( tweenID, textID ); }
		static inline void PauseTweenChar( unsigned int tweenID, unsigned int textID, unsigned int charID ) { AGKCommand1880( tweenID, textID, charID ); }
		static inline void ResumeTweenChar( unsigned int tweenID, unsigned int textID, unsigned int charID ) { AGKCommand1881( tweenID, textID, charID ); }
		static inline void PauseTweenObject( unsigned int tweenID, unsigned int objectID ) { AGKCommand1882( tweenID, objectID ); }
		static inline void ResumeTweenObject( unsigned int tweenID, unsigned int objectID ) { AGKCommand1883( tweenID, objectID ); }
		static inline void PauseTweenCamera( unsigned int tweenID, unsigned int cameraID ) { AGKCommand1884( tweenID, cameraID ); }
		static inline void ResumeTweenCamera( unsigned int tweenID, unsigned int cameraID ) { AGKCommand1885( tweenID, cameraID ); }
		static inline int GetNetworkExists( unsigned int iNetID ) { return AGKCommand1886( iNetID ); }
		static inline void SetObjectBlendModes( unsigned int objID, int src, int dst ) { AGKCommand1887( objID, src, dst ); }
		static inline int GetShaderExists( unsigned int shaderID ) { return AGKCommand1888( shaderID ); }
		static inline int GetTextAlignment( unsigned int iTextIndex ) { return AGKCommand1889( iTextIndex ); }
		static inline void SetupCloudData( const char* reserved ) { AGKCommand1890( reserved ); }
		static inline int GetCloudDataAllowed(  ) { return AGKCommand1891(  ); }
		static inline int GetCloudDataChanged(  ) { return AGKCommand1892(  ); }
		static inline char* GetCloudDataVariable( const char* varName, const char* defaultValue ) { return AGKCommand1893( varName, defaultValue ); }
		static inline void SetCloudDataVariable( const char* varName, const char* varValue ) { AGKCommand1894( varName, varValue ); }
		static inline void DeleteCloudDataVariable( const char* varName ) { AGKCommand1895( varName ); }
		static inline int GetDeviceNetworkType(  ) { return AGKCommand1896(  ); }
		static inline int GetStorageRemaining( const char * path ) { return AGKCommand1897( path ); }
		static inline int GetStorageTotal( const char * path ) { return AGKCommand1898( path ); }
		static inline int CheckPermission( const char* szPermission ) { return AGKCommand1899( szPermission ); }
		static inline void RequestPermission( const char* szPermission ) { AGKCommand1900( szPermission ); }
		static inline void DrawText( unsigned int iTextIndex ) { AGKCommand1901( iTextIndex ); }
		static inline void SetShadowCascadeValues( float cascade1, float cascade2, float cascade3 ) { AGKCommand1902( cascade1, cascade2, cascade3 ); }
		static inline void ARSetup(  ) { AGKCommand1903(  ); }
		static inline int ARGetStatus(  ) { return AGKCommand1904(  ); }
		static inline void ARDestroy(  ) { AGKCommand1905(  ); }
		static inline void ARControlCamera(  ) { AGKCommand1906(  ); }
		static inline void ARDrawBackground(  ) { AGKCommand1907(  ); }
		static inline void ARSetPlaneDetectionMode( int mode ) { AGKCommand1908( mode ); }
		static inline void ARSetLightEstimationMode( int mode ) { AGKCommand1909( mode ); }
		static inline float ARGetLightEstimate(  ) { return AGKCommand1910(  ); }
		static inline int ARHitTest( float screenX, float screenY ) { return AGKCommand1911( screenX, screenY ); }
		static inline float ARGetHitTestX( int index ) { return AGKCommand1912( index ); }
		static inline float ARGetHitTestY( int index ) { return AGKCommand1913( index ); }
		static inline float ARGetHitTestZ( int index ) { return AGKCommand1914( index ); }
		static inline float ARGetHitTestNormalX( int index ) { return AGKCommand1915( index ); }
		static inline float ARGetHitTestNormalY( int index ) { return AGKCommand1916( index ); }
		static inline float ARGetHitTestNormalZ( int index ) { return AGKCommand1917( index ); }
		static inline int ARGetHitTestType( int index ) { return AGKCommand1918( index ); }
		static inline void ARHitTestFinish(  ) { AGKCommand1919(  ); }
		static inline int ARGetPlanes( int reserved ) { return AGKCommand1920( reserved ); }
		static inline float ARGetPlaneX( int index ) { return AGKCommand1921( index ); }
		static inline float ARGetPlaneY( int index ) { return AGKCommand1922( index ); }
		static inline float ARGetPlaneZ( int index ) { return AGKCommand1923( index ); }
		static inline float ARGetPlaneAngleX( int index ) { return AGKCommand1924( index ); }
		static inline float ARGetPlaneAngleY( int index ) { return AGKCommand1925( index ); }
		static inline float ARGetPlaneAngleZ( int index ) { return AGKCommand1926( index ); }
		static inline float ARGetPlaneSizeX( int index ) { return AGKCommand1927( index ); }
		static inline float ARGetPlaneSizeZ( int index ) { return AGKCommand1928( index ); }
		static inline void ARGetPlanesFinish(  ) { AGKCommand1929(  ); }
		static inline int ARCreateAnchorFromHitTest( int index ) { return AGKCommand1930( index ); }
		static inline int ARCreateAnchorFromPlane( int index ) { return AGKCommand1931( index ); }
		static inline void ARFixObjectToAnchor( int objID, int anchorID ) { AGKCommand1932( objID, anchorID ); }
		static inline int ARGetAnchorStatus( int anchorID ) { return AGKCommand1933( anchorID ); }
		static inline void ARDeleteAnchor( int anchorID ) { AGKCommand1934( anchorID ); }
		static inline void DeleteObjectTree( unsigned int objID ) { AGKCommand1935( objID ); }
		static inline void FixSpriteToSkeleton2D( unsigned int spriteID, unsigned int iSkeletonID, int bone, int zorder ) { AGKCommand1936( spriteID, iSkeletonID, bone, zorder ); }
		static inline void SetRawTouchMoveSensitivity( int distance ) { AGKCommand1937( distance ); }
		static inline void Move3DPhysicsCharacterController( unsigned int objID, float x, float z, float velocity ) { AGKCommand1938( objID, x, z, velocity ); }
		static inline void Speak( const char * text, int delay ) { AGKCommand1939( text, delay ); }
		static inline void OverrideConsentAdMob( int consent ) { AGKCommand1940( consent ); }
		static inline void LoadConsentStatusAdMob( const char* szPubID, const char* privacyPolicy ) { AGKCommand1941( szPubID, privacyPolicy ); }
		static inline int GetConsentStatusAdMob(  ) { return AGKCommand1942(  ); }
		static inline void RequestConsentAdMob(  ) { AGKCommand1943(  ); }
		static inline int GetSupportedShaderVaryings(  ) { return AGKCommand1944(  ); }
		static inline int GetSpeechNumVoices(  ) { return AGKCommand1945(  ); }
		static inline char* GetSpeechVoiceLanguage( int index ) { return AGKCommand1946( index ); }
		static inline char* GetSpeechVoiceName( int index ) { return AGKCommand1947( index ); }
		static inline void OverrideConsentChartboost( int consent ) { AGKCommand1948( consent ); }
		static inline void AddHTTPHeader( unsigned int iHTTP, const char* headerName, const char* headerValue ) { AGKCommand1949( iHTTP, headerName, headerValue ); }
		static inline void RemoveHTTPHeader( unsigned int iHTTP, const char* headerName ) { AGKCommand1950( iHTTP, headerName ); }
		static inline int GetHTTPStatusCode( unsigned int iHTTP ) { return AGKCommand1951( iHTTP ); }
		static inline char* GetSpeechVoiceID( int index ) { return AGKCommand1952( index ); }
		static inline void SetSpeechLanguageByID( const char* sID ) { AGKCommand1953( sID ); }
		static inline int GetSpriteFlippedH( unsigned int iSpriteIndex ) { return AGKCommand1954( iSpriteIndex ); }
		static inline int GetSpriteInScreen( unsigned int iSpriteIndex ) { return AGKCommand1955( iSpriteIndex ); }
		static inline int GetSpriteFlippedV( unsigned int iSpriteIndex ) { return AGKCommand1956( iSpriteIndex ); }
		static inline void DrawParticles( unsigned int ID ) { AGKCommand1957( ID ); }
		static inline void Draw3DParticles( unsigned int ID ) { AGKCommand1958( ID ); }
		static inline void SetClipboardText( const char* szText ) { AGKCommand1959( szText ); }
		static inline char* GetClipboardText(  ) { return AGKCommand1960(  ); }
		static inline void SetEditBoxInputType( unsigned int index, int inputType ) { AGKCommand1961( index, inputType ); }
		static inline int GetExpansionFileError(  ) { return AGKCommand1962(  ); }
		static inline int GetSkeleton2DDepth( unsigned int iSkeleton ) { return AGKCommand1963( iSkeleton ); }
		static inline int GetEditBoxDepth( unsigned int index ) { return AGKCommand1964( index ); }
		static inline char* GetCharBuffer(  ) { return AGKCommand1965(  ); }
		static inline int GetCharBufferLength(  ) { return AGKCommand1966(  ); }
		static inline int GetRawJoystickSlider( unsigned int index, unsigned int slider ) { return AGKCommand1967( index, slider ); }
		static inline int GetRawJoystickPOV( unsigned int index, unsigned int pov ) { return AGKCommand1968( index, pov ); }
		static inline unsigned int CopyNetworkMessage( unsigned int iFromMsgID ) { return AGKCommand1969( iFromMsgID ); }
		static inline char* GetURLSchemeText(  ) { return AGKCommand1970(  ); }
		static inline void SetObjectAlpha( unsigned int objID, int alpha ) { AGKCommand1971( objID, alpha ); }
		static inline int GetObjectColorRed( int objID ) { return AGKCommand1972( objID ); }
		static inline int GetObjectColorGreen( int objID ) { return AGKCommand1973( objID ); }
		static inline int GetObjectColorBlue( int objID ) { return AGKCommand1974( objID ); }
		static inline int GetObjectAlpha( int objID ) { return AGKCommand1975( objID ); }
		static inline void ClearURLSchemeText(  ) { AGKCommand1976(  ); }
		static inline void ExtractZip( const char* zipfilename, const char* path, const char* password ) { AGKCommand1977( zipfilename, path, password ); }
		static inline unsigned int CreateObjectFromRawHeightMap( const char* szFilename, float width, float height, float length, int smoothing, int split, int rawWidth, int rawHeight ) { return AGKCommand1978( szFilename, width, height, length, smoothing, split, rawWidth, rawHeight ); }
		static inline void CreateObjectFromRawHeightMap( unsigned int objID, const char* szFilename, float width, float height, float length, int smoothing, int split, int rawWidth, int rawHeight ) { AGKCommand1979( objID, szFilename, width, height, length, smoothing, split, rawWidth, rawHeight ); }
		static inline void SetObjectMeshCollisionMode( unsigned int objID, int meshIndex, int mode ) { AGKCommand1980( objID, meshIndex, mode ); }
		static inline void SetObjectMeshVisible( unsigned int objID, unsigned int meshIndex, int mode ) { AGKCommand1981( objID, meshIndex, mode ); }
		static inline char* GetObjectTextureName( unsigned int objID, unsigned int textureIndex ) { return AGKCommand1982( objID, textureIndex ); }
		static inline unsigned int GetObjectNumTextures( unsigned int objID ) { return AGKCommand1983( objID ); }
		static inline char* GetRawJoystickName( unsigned int index ) { return AGKCommand1984( index ); }
		static inline void PlayYoutubeVideo( const char* developerKey, const char* videoID, float startTime ) { AGKCommand1985( developerKey, videoID, startTime ); }
		static inline void ShareFile( const char* szFilename ) { AGKCommand1986( szFilename ); }
		static inline int GetNetworkMessageFromPort( unsigned int iMsgID ) { return AGKCommand1987( iMsgID ); }
		static inline unsigned int CreateUDPListener( const char* ip, int port ) { return AGKCommand1988( ip, port ); }
		static inline int CreateUDPListener( unsigned int listenerID, const char* ip, int port ) { return AGKCommand1989( listenerID, ip, port ); }
		static inline void SendUDPNetworkMessage( unsigned int listenerID, unsigned int messageID, const char* toIP, int toPort ) { AGKCommand1990( listenerID, messageID, toIP, toPort ); }
		static inline unsigned int GetUDPNetworkMessage( unsigned int listenerID ) { return AGKCommand1991( listenerID ); }
		static inline void DeleteUDPListener( unsigned int listenerID ) { AGKCommand1992( listenerID ); }
		static inline void ExtractZipASync( const char* zipfilename, const char* path, const char* password ) { AGKCommand1993( zipfilename, path, password ); }
		static inline float GetZipExtractProgress(  ) { return AGKCommand1994(  ); }
		static inline int GetZipExtractComplete(  ) { return AGKCommand1995(  ); }
		static inline int GetWindowWidth(  ) { return AGKCommand1996(  ); }
		static inline int GetWindowHeight(  ) { return AGKCommand1997(  ); }
		static inline void CancelZipExtract(  ) { AGKCommand1998(  ); }
		static inline void GameCenterLogout(  ) { AGKCommand1999(  ); }
		static inline unsigned int GetImageSizeFromFile( const char* filename ) { return AGKCommand2000( filename ); }
		static inline void SetObjectMeshCastShadow( unsigned int objID, unsigned int meshIndex, int mode ) { AGKCommand2001( objID, meshIndex, mode ); }
		static inline void SetObjectShaderConstantArrayFloatByName( unsigned int objID, const char * szName, unsigned int arrayIndex, float value ) { AGKCommand2002( objID, szName, arrayIndex, value ); }
		static inline void SetObjectShaderConstantArrayVec2ByName( unsigned int objID, const char * szName, unsigned int arrayIndex, float value1, float value2 ) { AGKCommand2003( objID, szName, arrayIndex, value1, value2 ); }
		static inline void SetObjectShaderConstantArrayVec3ByName( unsigned int objID, const char * szName, unsigned int arrayIndex, float value1, float value2, float value3 ) { AGKCommand2004( objID, szName, arrayIndex, value1, value2, value3 ); }
		static inline void SetObjectShaderConstantArrayVec4ByName( unsigned int objID, const char * szName, unsigned int arrayIndex, float value1, float value2, float value3, float value4 ) { AGKCommand2005( objID, szName, arrayIndex, value1, value2, value3, value4 ); }
		static inline void SetShaderErrorMode( unsigned int mode ) { AGKCommand2006( mode ); }
		static inline void SetShaderConstantArrayFloatByName( unsigned int shaderID, const char * szName, unsigned int arrayIndex, float value ) { AGKCommand2007( shaderID, szName, arrayIndex, value ); }
		static inline void SetShaderConstantArrayVec2ByName( unsigned int shaderID, const char * szName, unsigned int arrayIndex, float value1, float value2 ) { AGKCommand2008( shaderID, szName, arrayIndex, value1, value2 ); }
		static inline void SetShaderConstantArrayVec3ByName( unsigned int shaderID, const char * szName, unsigned int arrayIndex, float value1, float value2, float value3 ) { AGKCommand2009( shaderID, szName, arrayIndex, value1, value2, value3 ); }
		static inline void SetShaderConstantArrayVec4ByName( unsigned int shaderID, const char * szName, unsigned int arrayIndex, float value1, float value2, float value3, float value4 ) { AGKCommand2010( shaderID, szName, arrayIndex, value1, value2, value3, value4 ); }
		static inline float GetObject3DPhysicsMaxLinearVelocity( unsigned int objID ) { return AGKCommand2011( objID ); }
		static inline int IsInvertedDepth(  ) { return AGKCommand2012(  ); }
		static inline int GetClipSpaceMode(  ) { return AGKCommand2013(  ); }
		static inline int IsTopLeftOrigin(  ) { return AGKCommand2014(  ); }
		static inline float GetDrawing3DSetupTime(  ) { return AGKCommand2015(  ); }
		static inline unsigned int MakeColor( unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha ) { return AGKCommand2016( red, green, blue, alpha ); }
		static inline unsigned int GetColorAlpha( unsigned int color ) { return AGKCommand2017( color ); }
		static inline void SetPresentMode( int mode ) { AGKCommand2018( mode ); }
		static inline void ForcePresent(  ) { AGKCommand2019(  ); }
		static inline int GetAppInstalled( const char * packageName ) { return AGKCommand2020( packageName ); }
		static inline void SetSnapChatStickerSettings( float x, float y, int width, int height, float angle ) { AGKCommand2021( x, y, width, height, angle ); }
		static inline void ShareSnapChatImage( const char* imageFile, const char* stickerFile, const char* caption, const char* url ) { AGKCommand2022( imageFile, stickerFile, caption, url ); }
		static inline void SetLocalNotification( int iID, int datetime, const char * szMessage, const char * szDeepLink ) { AGKCommand2023( iID, datetime, szMessage, szDeepLink ); }
		static inline void AddVulkanDeviceExtensions( const char* szExtensions ) { AGKCommand2024( szExtensions ); }
		static inline void AddVulkanInstanceExtensions( const char* szExtensions ) { AGKCommand2025( szExtensions ); }
		static inline char* GetRendererName(  ) { return AGKCommand2026(  ); }
		static inline unsigned int GetOpenGLImageID( unsigned int iImageIndex ) { return AGKCommand2027( iImageIndex ); }
		static inline void* GetVulkanVRImageData( unsigned int iImageIndex ) { return AGKCommand2028( iImageIndex ); }
		static inline void SetVRImage( unsigned int iImageIndex, int mode ) { AGKCommand2029( iImageIndex, mode ); }
		static inline char* GetNetworkClientIP( unsigned int iNetID, unsigned int client ) { return AGKCommand2030( iNetID, client ); }
		static inline char* GetNetworkServerIP( unsigned int iNetID ) { return AGKCommand2031( iNetID ); }
		static inline int GetRawMouseFourthPressed(  ) { return AGKCommand2032(  ); }
		static inline int GetRawMouseFourthState(  ) { return AGKCommand2033(  ); }
		static inline int GetRawMouseFourthReleased(  ) { return AGKCommand2034(  ); }
		static inline int GetRawMouseFifthPressed(  ) { return AGKCommand2035(  ); }
		static inline int GetRawMouseFifthState(  ) { return AGKCommand2036(  ); }
		static inline int GetRawMouseFifthReleased(  ) { return AGKCommand2037(  ); }
		static inline void SetSoundDeviceMode( int mode ) { AGKCommand2038( mode ); }
		static inline void AddNetworkMessageByte( unsigned int iMsgID, unsigned int value ) { AGKCommand2039( iMsgID, value ); }
		static inline unsigned int GetNetworkMessageByte( unsigned int iMsgID ) { return AGKCommand2040( iMsgID ); }
		static inline float GetCameraWorldX( unsigned int cameraID ) { return AGKCommand2041( cameraID ); }
		static inline float GetCameraWorldY( unsigned int cameraID ) { return AGKCommand2042( cameraID ); }
		static inline float GetCameraWorldZ( unsigned int cameraID ) { return AGKCommand2043( cameraID ); }
		static inline void FixCameraToObject( unsigned int cameraID, unsigned int objID ) { AGKCommand2044( cameraID, objID ); }
		static inline void ResetVRImages(  ) { AGKCommand2045(  ); }
		static inline void SetSpriteBlendModes( unsigned int iSpriteIndex, int src, int dst ) { AGKCommand2046( iSpriteIndex, src, dst ); }
		static inline void SetAdMobChildRating( int rating ) { AGKCommand2047( rating ); }
		static inline int GetRewardAdValueAdMob(  ) { return AGKCommand2048(  ); }
		static inline char* GetMemblockSHA1( unsigned int memID ) { return AGKCommand2049( memID ); }
		static inline char* GetMemblockSHA256( unsigned int memID ) { return AGKCommand2050( memID ); }
		static inline void CreateImageFromPNGMemblock( unsigned int imageID, unsigned int memID ) { AGKCommand2051( imageID, memID ); }
		static inline unsigned int CreateImageFromPNGMemblock( unsigned int memID ) { return AGKCommand2052( memID ); }
		static inline void CreateSoundFromOGGMemblock( unsigned int soundID, unsigned int memID ) { AGKCommand2053( soundID, memID ); }
		static inline unsigned int CreateSoundFromOGGMemblock( unsigned int memID ) { return AGKCommand2054( memID ); }
		static inline void CreateMusicFromOGGMemblock( unsigned int musicID, unsigned int memID ) { AGKCommand2055( musicID, memID ); }
		static inline unsigned int CreateMusicFromOGGMemblock( unsigned int memID ) { return AGKCommand2056( memID ); }
		static inline void SetImageSubImages( unsigned int iImageIndex, const char* sSubImageFile ) { AGKCommand2057( iImageIndex, sSubImageFile ); }
		static inline int GetInAppPurchaseAvailable2( int iID ) { return AGKCommand2058( iID ); }
		static inline void InAppPurchaseResetPurchase( const char* token ) { AGKCommand2059( token ); }
		static inline char* GetInAppPurchaseToken( int iID ) { return AGKCommand2060( iID ); }
		static inline void FixObjectToCamera( unsigned int objID, unsigned int toCameraID ) { AGKCommand2061( objID, toCameraID ); }
		static inline float GetCameraWorldAngleX( unsigned int cameraID ) { return AGKCommand2062( cameraID ); }
		static inline float GetCameraWorldAngleY( unsigned int cameraID ) { return AGKCommand2063( cameraID ); }
		static inline float GetCameraWorldAngleZ( unsigned int cameraID ) { return AGKCommand2064( cameraID ); }
		static inline float GetCameraWorldQuatW( unsigned int cameraID ) { return AGKCommand2065( cameraID ); }
		static inline float GetCameraWorldQuatX( unsigned int cameraID ) { return AGKCommand2066( cameraID ); }
		static inline float GetCameraWorldQuatY( unsigned int cameraID ) { return AGKCommand2067( cameraID ); }
		static inline float GetCameraWorldQuatZ( unsigned int cameraID ) { return AGKCommand2068( cameraID ); }
		static inline int GetErrorMode(  ) { return AGKCommand2069(  ); }
		static inline void SetSpritePhysicsGravityScale( unsigned int iSpriteIndex, float scale ) { AGKCommand2070( iSpriteIndex, scale ); }
		static inline float GetSpritePhysicsGravityScale( unsigned int iSpriteIndex ) { return AGKCommand2071( iSpriteIndex ); }
		static inline void SetSpritePhysicsInitiallyAwake( unsigned int iSpriteIndex, int awake ) { AGKCommand2072( iSpriteIndex, awake ); }
		static inline void SetSpritePhysicsAllowSleep( unsigned int iSpriteIndex, int sleep ) { AGKCommand2073( iSpriteIndex, sleep ); }
		static inline float GetSpritePhysicsInertia( unsigned int iSpriteIndex ) { return AGKCommand2074( iSpriteIndex ); }
		static inline int GetSpritePhysicsIsBullet( unsigned int iSpriteIndex ) { return AGKCommand2075( iSpriteIndex ); }
		static inline int GetSpritePhysicsIsAwake( unsigned int iSpriteIndex ) { return AGKCommand2076( iSpriteIndex ); }
		static inline float GetTweenCustomTime( unsigned int tweenID ) { return AGKCommand2077( tweenID ); }
		static inline float GetTweenCustomEndTime( unsigned int tweenID ) { return AGKCommand2078( tweenID ); }
		static inline void ClearTweenSprite( unsigned int tweenID ) { AGKCommand2079( tweenID ); }
		static inline void ClearTweenCustom( unsigned int tweenID ) { AGKCommand2080( tweenID ); }
		static inline void ClearTweenText( unsigned int tweenID ) { AGKCommand2081( tweenID ); }
		static inline void ClearTweenChar( unsigned int tweenID ) { AGKCommand2082( tweenID ); }
		static inline void ClearTweenObject( unsigned int tweenID ) { AGKCommand2083( tweenID ); }
		static inline void ClearTweenCamera( unsigned int tweenID ) { AGKCommand2084( tweenID ); }
		static inline float GetTweenSpriteTime( unsigned int tweenID, unsigned int spriteID ) { return AGKCommand2085( tweenID, spriteID ); }
		static inline float GetTweenSpriteEndTime( unsigned int tweenID, unsigned int spriteID ) { return AGKCommand2086( tweenID, spriteID ); }
		static inline float GetTweenTextTime( unsigned int tweenID, unsigned int textID ) { return AGKCommand2087( tweenID, textID ); }
		static inline float GetTweenTextEndTime( unsigned int tweenID, unsigned int textID ) { return AGKCommand2088( tweenID, textID ); }
		static inline float GetTweenCharTime( unsigned int tweenID, unsigned int textID, unsigned int charID ) { return AGKCommand2089( tweenID, textID, charID ); }
		static inline float GetTweenCharEndTime( unsigned int tweenID, unsigned int textID, unsigned int charID ) { return AGKCommand2090( tweenID, textID, charID ); }
		static inline float GetTweenObjectTime( unsigned int tweenID, unsigned int objectID ) { return AGKCommand2091( tweenID, objectID ); }
		static inline float GetTweenObjectEndTime( unsigned int tweenID, unsigned int objectID ) { return AGKCommand2092( tweenID, objectID ); }
		static inline void SetupNoise( float frequency, float amplitude, float lacunarity, float persistence ) { AGKCommand2093( frequency, amplitude, lacunarity, persistence ); }
		static inline float GetNoiseX( float x ) { return AGKCommand2094( x ); }
		static inline float GetNoiseXY( float x, float y ) { return AGKCommand2095( x, y ); }
		static inline float GetNoiseXYZ( float x, float y, float z ) { return AGKCommand2096( x, y, z ); }
		static inline float GetFractalX( unsigned int octaves, float x ) { return AGKCommand2097( octaves, x ); }
		static inline float GetFractalXY( unsigned int octaves, float x, float y ) { return AGKCommand2098( octaves, x, y ); }
		static inline float GetFractalXYZ( unsigned int octaves, float x, float y, float z ) { return AGKCommand2099( octaves, x, y, z ); }
		static inline void SetTextShader( unsigned int iTextIndex, unsigned int shaderID ) { AGKCommand2100( iTextIndex, shaderID ); }
		static inline void SetTextShaderConstantByName( unsigned int iTextIndex, const char* szName, float value1, float value2, float value3, float value4 ) { AGKCommand2101( iTextIndex, szName, value1, value2, value3, value4 ); }
		static inline void SetTextShaderConstantArrayByName( unsigned int iTextIndex, const char* szName, unsigned int arrayIndex, float value1, float value2, float value3, float value4 ) { AGKCommand2102( iTextIndex, szName, arrayIndex, value1, value2, value3, value4 ); }
		static inline void SetTextShaderConstantDefault( unsigned int iTextIndex, const char* szName ) { AGKCommand2103( iTextIndex, szName ); }
		static inline void InAppPurchaseRedeemOffer(  ) { AGKCommand2104(  ); }
		static inline int ExternalSDKSupported( const char* sdk ) { return AGKCommand2105( sdk ); }
		static inline void ExternalCommand( const char* sdk, const char* command, const char* str1, const char* str2 ) { AGKCommand2106( sdk, command, str1, str2 ); }
		static inline int ExternalCommandInt( const char* sdk, const char* command, const char* str1, const char* str2 ) { return AGKCommand2107( sdk, command, str1, str2 ); }
		static inline float ExternalCommandFloat( const char* sdk, const char* command, const char* str1, const char* str2 ) { return AGKCommand2108( sdk, command, str1, str2 ); }
		static inline char* ExternalCommandString( const char* sdk, const char* command, const char* str1, const char* str2 ) { return AGKCommand2109( sdk, command, str1, str2 ); }
		static inline char* GetAppReceipt(  ) { return AGKCommand2110(  ); }
		static inline void InAppPurchaseActivateWithPlan( int iID, const char* planToken ) { AGKCommand2111( iID, planToken ); }
		static inline int GetInAppPurchaseSubNumPlans( int iID ) { return AGKCommand2112( iID ); }
		static inline int GetInAppPurchaseSubPlanNumPeriods( int iID, int planIndex ) { return AGKCommand2113( iID, planIndex ); }
		static inline char* GetInAppPurchaseSubPlanPrice( int iID, int planIndex, int periodIndex ) { return AGKCommand2114( iID, planIndex, periodIndex ); }
		static inline int GetInAppPurchaseSubPlanDuration( int iID, int planIndex, int periodIndex ) { return AGKCommand2115( iID, planIndex, periodIndex ); }
		static inline char* GetInAppPurchaseSubPlanDurationUnit( int iID, int planIndex, int periodIndex ) { return AGKCommand2116( iID, planIndex, periodIndex ); }
		static inline int GetInAppPurchaseSubPlanPaymentType( int iID, int planIndex, int periodIndex ) { return AGKCommand2117( iID, planIndex, periodIndex ); }
		static inline char* GetInAppPurchaseSubPlanTags( int iID, int planIndex ) { return AGKCommand2118( iID, planIndex ); }
		static inline char* GetInAppPurchaseSubPlanToken( int iID, int planIndex ) { return AGKCommand2119( iID, planIndex ); }
		static inline int GetDisplayNumCutouts(  ) { return AGKCommand2120(  ); }
		static inline float GetDisplayCutoutTop( int index ) { return AGKCommand2121( index ); }
		static inline float GetDisplayCutoutBottom( int index ) { return AGKCommand2122( index ); }
		static inline float GetDisplayCutoutLeft( int index ) { return AGKCommand2123( index ); }
		static inline float GetDisplayCutoutRight( int index ) { return AGKCommand2124( index ); }
		static inline float GetScreenBoundsSafeTop(  ) { return AGKCommand2125(  ); }
		static inline float GetScreenBoundsSafeBottom(  ) { return AGKCommand2126(  ); }
		static inline float GetScreenBoundsSafeLeft(  ) { return AGKCommand2127(  ); }
		static inline float GetScreenBoundsSafeRight(  ) { return AGKCommand2128(  ); }
		static inline int IsPinAppAvailable(  ) { return AGKCommand2129(  ); }
		static inline void PinApp( int enable ) { AGKCommand2130( enable ); }
		static inline int IsDarkTheme(  ) { return AGKCommand2131(  ); }
		static inline int GetInAppPurchaseIsRenewing( int iID ) { return AGKCommand2132( iID ); }
};

#endif
