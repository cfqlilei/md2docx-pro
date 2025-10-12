# 单文件转换API测试

这是一个用于测试单文件转换API的Markdown文档。

## 功能验证

### 文本格式
- **粗体文本**
- *斜体文本*
- `行内代码`
- ~~删除线~~

### 列表
1. 有序列表项1
2. 有序列表项2
3. 有序列表项3

- 无序列表项A
- 无序列表项B
- 无序列表项C

### 代码块
```python
def api_test():
    print("API测试函数")
    return {"status": "success"}
```

### 表格
| API端点 | 方法 | 状态 |
|---------|------|------|
| /api/health | GET | ✅ |
| /api/convert/single | POST | ✅ |
| /api/convert/batch | POST | ✅ |
| /api/config | GET | ✅ |

## 测试结论
单文件转换API功能正常。
