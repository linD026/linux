#include <linux/init.h>
#include <linux/sysctl.h>
#include <linux/pgtable.h>

int sysctl_cow_pte_enabled;
DEFINE_STATIC_KEY_FALSE(cow_pte_enabled);

static inline void update_cow_pte_key(void)
{
	if (sysctl_cow_pte_enabled)
		static_branch_disable(&cow_pte_enabled);
	else
		static_branch_enable(&cow_pte_enabled);

	pr_info("sysctl_cow_pte_enabled=%d\n", sysctl_cow_pte_enabled);
}

int cow_pte_handler(struct ctl_table *table, int write,
		void *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;

	ret = proc_dointvec_minmax(table, write, buffer, lenp, ppos);

	update_cow_pte_key();

	return ret;
}

static int __init init_cow_pte(void)
{
	update_cow_pte_key();

	return 0;
}
pure_initcall(init_cow_pte);
