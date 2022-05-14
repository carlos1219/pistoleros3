#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define PERROR(a) \
    {             \
        LPVOID lpMsgBuf;                                         \
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |           \
                   FORMAT_MESSAGE_FROM_SYSTEM |                  \
                   FORMAT_MESSAGE_IGNORE_INSERTS, NULL,          \
                   GetLastError(),                               \
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),    \
                   (LPTSTR) &lpMsgBuf,0,NULL );                  \
        fprintf(stderr,"%s:(%d)%s\n",a,GetLastError(),lpMsgBuf); \
        LocalFree( lpMsgBuf );                                   \
    }    

#ifdef DLL_EXPORTS
extern "C" DLL_API int PIST_inicio(unsigned int nPistoleros, int ret,
    int semilla);
extern "C" DLL_API int PIST_nuevoPistolero(char pist);
extern "C" DLL_API char PIST_vIctima(void);
extern "C" DLL_API int PIST_disparar(char pist);
extern "C" DLL_API int PIST_morirme(void);
extern "C" DLL_API int PIST_fin(void);
extern "C" DLL_API int refrescar(void);
extern "C" DLL_API void pon_error(char* mensaje);
#endif
