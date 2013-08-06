/*
 * System calls
 *
 *    Francesco Salvestrini <f.salvestrini@nextworks.it>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/stringify.h>
#include <linux/uaccess.h>

#define RINA_PREFIX "syscalls"

#include "logs.h"
#include "utils.h"
#include "personality.h"
#include "debug.h"
#include "ipcp-utils.h"
#include "du.h"

#define CALL_PERSONALITY(RETVAL, PERS, HOOK, ARGS...)                   \
        do {                                                            \
                LOG_DBG("Handling personality hook %s",                 \
                        __stringify(HOOK));                             \
                                                                        \
                if (PERS == NULL) {                                     \
                        LOG_ERR("No personality registered");           \
                        return -1;                                      \
                }                                                       \
                                                                        \
                ASSERT(PERS);                                           \
                ASSERT(PERS -> ops);                                    \
                                                                        \
                if (PERS -> ops -> HOOK == NULL) {                      \
                        LOG_ERR("Personality has no %s hook",           \
                                __stringify(HOOK));                     \
                        return -1;                                      \
                }                                                       \
                                                                        \
                LOG_DBG("Calling personality hook %s",                  \
                        __stringify(HOOK));                             \
                                                                        \
                RETVAL = PERS -> ops -> HOOK (PERS->data, ##ARGS);      \
        } while (0)

#define CALL_DEFAULT_PERSONALITY(RETVAL, HOOK, ARGS...)                 \
        CALL_PERSONALITY(RETVAL, default_personality, HOOK, ##ARGS)

SYSCALL_DEFINE3(ipc_create,
                const struct name __user *, name,
                ipc_process_id_t,           id,
                const char __user *,        type)
{
        long          retval;
        struct name * tn;
        char *        tt;

        tn = name_dup_from_user(name);
        if (!tn)
                return -EFAULT;
        tt = strdup_from_user(type);
        if (!tt) {
                name_destroy(tn);
                return -EFAULT;
        }
        CALL_DEFAULT_PERSONALITY(retval, ipc_create, tn, id, tt);

        name_destroy(tn);
        rkfree(tt);

        return retval;
}

SYSCALL_DEFINE2(ipc_configure,
                ipc_process_id_t,                  id,
                const struct ipcp_config __user *, config)
{
        long retval;

        LOG_MISSING; /* FIXME: Rearrange for copy_from_user() */

        CALL_DEFAULT_PERSONALITY(retval, ipc_configure, id, config);

        return retval;
}

SYSCALL_DEFINE1(ipc_destroy,
                ipc_process_id_t, id)
{
        long retval;

        CALL_DEFAULT_PERSONALITY(retval, ipc_destroy, id);

        return retval;
}

SYSCALL_DEFINE3(sdu_read,
                port_id_t,     id,
                void __user *, buffer,
                size_t,        size)
{
        ssize_t      retval;
        struct sdu * tmp;

        tmp = NULL;

        CALL_DEFAULT_PERSONALITY(retval, sdu_read, id, tmp);
        /* NOTE: We must receive SDU ownership ... */

        if (!retval) {
                if (!sdu_is_ok(tmp))
                        return -EFAULT;

                /* NOTE: We don't handle partial copies now ... */
                if (tmp->buffer->size > size) {
                        sdu_destroy(tmp);
                        return -EFAULT;
                }
                if (copy_to_user(buffer,
                                 tmp->buffer->data,
                                 tmp->buffer->size)) {
                        sdu_destroy(tmp);
                        return -EFAULT;
                }

                sdu_destroy(tmp);

                return tmp->buffer->size;
        }

        return -EFAULT;
}

SYSCALL_DEFINE3(sdu_write,
                port_id_t,           id,
                const void __user *, buffer,
                size_t,              size)
{
        ssize_t      retval;
        struct sdu * sdu;
        void *       tmp_buffer;

        if (!buffer || !size)
                return -EFAULT;

        tmp_buffer = rkmalloc(size, GFP_KERNEL);
        if (!tmp_buffer)
                return -EFAULT;
        if (copy_from_user(tmp_buffer, buffer, size)) {
                rkfree(tmp_buffer);
                return -EFAULT;
        }

        sdu = sdu_create_from(tmp_buffer, size);
        if (!sdu) {
                rkfree(tmp_buffer);
                return -EFAULT;
        }
        ASSERT(sdu_is_ok(sdu));

        /* NOTE: SDU ownership will be passed to the internal layers ... */
        CALL_DEFAULT_PERSONALITY(retval, sdu_write, id, sdu);

        return sdu->buffer->size;
}

SYSCALL_DEFINE1(connection_create,
                const struct connection __user *, connection)
{
        long              retval;
        struct connection tmp;

        if (copy_from_user(&tmp, connection, sizeof(*connection)))
                return -EFAULT;

        CALL_DEFAULT_PERSONALITY(retval, connection_create, &tmp);

        return retval;
}

SYSCALL_DEFINE1(connection_destroy,
                cep_id_t, id)
{
        long retval;
        
        CALL_DEFAULT_PERSONALITY(retval, connection_destroy, id);
        
        return retval;
}

SYSCALL_DEFINE2(connection_update,
                cep_id_t, from_id,
                cep_id_t, to_id)
{
        long retval;

        CALL_DEFAULT_PERSONALITY(retval, connection_update, from_id, to_id);

        return retval;
}
