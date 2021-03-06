/*
 * Skeleton plugin
 *
 *    Vincenzo Maffione <v.maffione@nextworks.it>
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

#include <linux/export.h>
#include <linux/module.h>
#include <linux/string.h>

#define RINA_PREFIX "skeleton"

#include "logs.h"
#include "rds/rmem.h"
#include "rmt-ps.h"

static void skeleton_max_q_policy_tx(struct rmt_ps *      ps,
                                     struct pdu *         pdu,
                                     struct rmt_n1_port * port)
{ printk("%s: called()\n", __func__); }

static void skeleton_max_q_policy_rx(struct rmt_ps *      ps,
                                     struct sdu *         sdu,
                                     struct rmt_n1_port * port)
{ printk("%s: called()\n", __func__); }

static void skeleton_rmt_q_monitor_policy_tx(struct rmt_ps *      ps,
                                             struct pdu *         pdu,
                                             struct rmt_n1_port * port)
{ printk("%s: called()\n", __func__); }

static void skeleton_rmt_q_monitor_policy_rx(struct rmt_ps *      ps,
                                             struct sdu *         sdu,
                                             struct rmt_n1_port * port)
{ printk("%s: called()\n", __func__); }

static struct pdu *
skeleton_rmt_next_scheduled_policy_tx(struct rmt_ps *      ps,
                                      struct rmt_n1_port * port)
{
        printk("%s: called()\n", __func__);
        return NULL;
}

static int skeleton_rmt_enqueue_scheduling_policy_tx(struct rmt_ps *      ps,
                                                     struct rmt_n1_port * port,
                                                     struct pdu *         pdu)
{
        printk("%s: called()\n", __func__);
        return 0;
}

static int skeleton_rmt_scheduling_create_policy_tx(struct rmt_ps *      ps,
                                                    struct rmt_n1_port * port)
{ printk("%s: called()\n", __func__);
  return 0;
}

static int skeleton_rmt_scheduling_destroy_policy_tx(struct rmt_ps *      ps,
                                                     struct rmt_n1_port * port)
{
        printk("%s: called()\n", __func__);
        return 0;
}

static int rmt_ps_set_policy_set_param(struct ps_base * bps,
                                       const char    * name,
                                       const char    * value)
{
        struct rmt_ps *ps = container_of(bps, struct rmt_ps, base);

        (void) ps;

        if (!name) {
                LOG_ERR("Null parameter name");
                return -1;
        }

        if (!value) {
                LOG_ERR("Null parameter value");
                return -1;
        }

        LOG_ERR("No such parameter to set");

        return -1;
}

static struct ps_base *
rmt_ps_skeleton_create(struct rina_component * component)
{
        struct rmt * rmt = rmt_from_component(component);
        struct rmt_ps * ps = rkzalloc(sizeof(*ps), GFP_KERNEL);

        if (!ps) {
                return NULL;
        }

        ps->base.set_policy_set_param = rmt_ps_set_policy_set_param;
        ps->dm = rmt;
        ps->priv = NULL;
        ps->max_q_policy_tx = skeleton_max_q_policy_tx;
        ps->max_q_policy_rx = skeleton_max_q_policy_rx;
        ps->rmt_q_monitor_policy_tx = skeleton_rmt_q_monitor_policy_tx;
        ps->rmt_q_monitor_policy_rx = skeleton_rmt_q_monitor_policy_rx;
        ps->rmt_next_scheduled_policy_tx     = skeleton_rmt_next_scheduled_policy_tx;
        ps->rmt_enqueue_scheduling_policy_tx = skeleton_rmt_enqueue_scheduling_policy_tx;
        ps->rmt_scheduling_create_policy_tx  = skeleton_rmt_scheduling_create_policy_tx;
        ps->rmt_scheduling_destroy_policy_tx = skeleton_rmt_scheduling_destroy_policy_tx;

        return &ps->base;
}

static void rmt_ps_skeleton_destroy(struct ps_base * bps)
{
        struct rmt_ps *ps = container_of(bps, struct rmt_ps, base);

        if (bps) {
                rkfree(ps);
        }
}

static struct ps_factory factory = {
        .owner          = THIS_MODULE,
        .create  = rmt_ps_skeleton_create,
        .destroy = rmt_ps_skeleton_destroy,
};

#define RINA_SKELETON_NAME   "skeleton"

static int __init mod_init(void)
{
        int ret;

        strcpy(factory.name, RINA_SKELETON_NAME);

        ret = rmt_ps_publish(&factory);
        if (ret) {
                LOG_ERR("Failed to publish policy set factory");
                return -1;
        }

        LOG_INFO("RMT skeleton policy set loaded successfully");

        return 0;
}

static void __exit mod_exit(void)
{
        int ret = rmt_ps_unpublish(RINA_SKELETON_NAME);

        if (ret) {
                LOG_ERR("Failed to unpublish policy set factory");
                return;
        }

        LOG_INFO("RMT skeleton policy set unloaded successfully");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_DESCRIPTION("RMT skeleton policy set");

MODULE_LICENSE("GPL");

MODULE_AUTHOR("Vincenzo Maffione <v.maffione@nextworks.it>");
MODULE_AUTHOR("Francesco Salvestrini <f.salvestrini@nextworks.it>");
