// SPDX-License-Identifier: Dual BSD/GPL
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

MODULE_AUTHOR("Vashchishin Ilya <illa123fgh@gmail.com>");
MODULE_DESCRIPTION("Hello, World in Linux Kernel Module");
MODULE_LICENSE("Dual BSD/GPL");

static uint print_count = 1;
module_param(print_count, uint, 0444);
MODULE_PARM_DESC(print_count, "Number of 'Hello, world!' messages to print");

struct hello_entry {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(hello_list);

static int __init hello_init(void)
{
	int i;
	struct hello_entry *item;

	if (print_count == 0 || (print_count >= 5 && print_count <= 10))
		pr_warn("Warning: print_count is %u\n", print_count);
	else if (print_count > 10)
		BUG_ON(1);

	for (i = 0; i < print_count; i++) {
		if (i == 5) {
			pr_err("Simulating kmalloc() failure for the 5th entry.\n");
			item = NULL;
		} else {
			item = kmalloc(sizeof(*item), GFP_KERNEL);
		}

		BUG_ON(!item);

		item->time = ktime_get();
		list_add(&item->list, &hello_list);
		pr_info("Hello, world! (%d/%u)\n", i + 1, print_count);
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_entry *item, *tmp;

	list_for_each_entry_safe(item, tmp, &hello_list, list) {
		pr_info("Time: %lld ns\n", ktime_to_ns(item->time));
		list_del(&item->list);
		kfree(item);
	}
}

module_init(hello_init);
module_exit(hello_exit);

