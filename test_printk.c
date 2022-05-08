#include <linux/module.h>
#include <linux/init.h>

int main() {
  printk(KERN_ALERT, "Hello printk\n");
  return 0;
}
