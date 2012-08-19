#ifndef __ASM_ARCH_RMOBILE_H
#define __ASM_ARCH_RMOBILE_H

#if defined(CONFIG_RMOBILE)
#if defined(CONFIG_SH73A0)
#include <asm/arch/sh73a0.h>
#else
#error "SOC Name not defined"
#endif
#endif /* CONFIG_RMOBILE */

#endif /* __ASM_ARCH_RMOBILE_H */
