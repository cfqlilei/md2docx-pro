# Markdown转Word工具测试 - 包含图片

## 功能介绍

这是一个测试文档，用于验证图片嵌入功能。

### 在线图片测试

下面是一个在线图片：

![GitHub Logo](https://github.githubassets.com/images/modules/logos_page/GitHub-Mark.png)

### 本地图片测试

如果有本地图片，也会被嵌入到Word文档中。

## 其他内容

### 代码块

```python
def convert_markdown_to_docx():
    print("Converting with embedded resources...")
    return True
```

### 表格

| 功能 | 状态 | 说明 |
|------|------|------|
| 图片嵌入 | ✅ 支持 | 使用--embed-resources参数 |
| 在线图片 | ✅ 支持 | 自动下载并嵌入 |
| 本地图片 | ✅ 支持 | 直接嵌入文档 |

### 列表

1. **图片嵌入功能**
   - 支持在线图片
   - 支持本地图片
   - 自动处理图片格式

2. **转换质量**
   - 保持原始格式
   - 优化文件大小
   - 确保兼容性

## 总结

使用`--embed-resources`参数可以确保所有图片和资源都被正确嵌入到Word文档中，无需依赖外部文件。

**测试时间**: 2024年10月13日
