# 测试命令
```bash
make all
kd_tree_demo.exe
ips_generator.exe <parameters>
ips_query.exe
make clean
```

## ips_generator  命令参数
```bash
ips_generator.exe [-v <version>] [-n <count>]
```
-v: 取值为 4 / 6，指定生成 IP 的版本，默认值为 4

-n: 取值为非负整数，指定生成 IP 的个数，不保证无相等，默认值为 100000