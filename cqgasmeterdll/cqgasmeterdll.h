// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� CQGASMETERDLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// CQGASMETERDLL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef CQGASMETERDLL_EXPORTS
#define CQGASMETERDLL_API __declspec(dllexport)
#else
#define CQGASMETERDLL_API __declspec(dllimport)
#endif

CQGASMETERDLL_API int PushFile(const char *lpPathName);

CQGASMETERDLL_API int PullFile(const char *lpPathName);
