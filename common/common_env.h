/// <summary>
/// ��־ʵ��ͷ�ļ�
/// </summary>
/// <creater>wpj</creater>
/// <version>1.0</version>
/// <revision>
///	<record part="" date="" description=""  />
/// </revision>
#ifndef _COMMON_ENV_H
#define _COMMON_ENV_H
#include<stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

/// ��ȡӦ�ó���ִ�о��·��
const char* GetApplicationPath(char *buf, const int count);
///�����Ƿ�������
bool IsAppRunning(const char*, const int);
// �����߳�����
void SleepMs(const int);
// �����ͷŴ�Ϊ�޷������
const unsigned int ChangeToUINT(const float, const unsigned char);
// �����ͷŴ�Ϊ����
const int ChangeToINT(const float, const unsigned char);

#ifdef __cplusplus
}
#endif

#endif  /* _COMMON_ENV_H */
