package models

import "time"

// ConversionRequest 单文件转换请求
type ConversionRequest struct {
	InputFile    string `json:"input_file"`    // 输入Markdown文件路径
	OutputDir    string `json:"output_dir"`    // 输出目录路径（可选）
	OutputName   string `json:"output_name"`   // 输出文件名（不含扩展名，可选）
	TemplateFile string `json:"template_file"` // 参考模板文件路径（可选）
}

// BatchConversionRequest 批量转换请求
type BatchConversionRequest struct {
	InputFiles   []string `json:"input_files"`   // 输入Markdown文件路径列表
	OutputDir    string   `json:"output_dir"`    // 统一输出目录路径（可选）
	TemplateFile string   `json:"template_file"` // 参考模板文件路径（可选）
}

// ConversionResponse 转换响应
type ConversionResponse struct {
	Success    bool                  `json:"success"`
	Message    string                `json:"message"`
	OutputFile string                `json:"output_file,omitempty"` // 单文件转换时的输出文件路径
	Results    []ConversionResult    `json:"results,omitempty"`     // 批量转换时的结果列表
	Error      string                `json:"error,omitempty"`
}

// ConversionResult 单个文件的转换结果
type ConversionResult struct {
	InputFile  string `json:"input_file"`
	OutputFile string `json:"output_file"`
	Success    bool   `json:"success"`
	Error      string `json:"error,omitempty"`
}

// ConversionStatus 转换状态
type ConversionStatus struct {
	ID          string    `json:"id"`
	Status      string    `json:"status"` // "pending", "processing", "completed", "failed"
	Progress    int       `json:"progress"` // 0-100
	Message     string    `json:"message"`
	StartTime   time.Time `json:"start_time"`
	EndTime     *time.Time `json:"end_time,omitempty"`
	InputFiles  []string  `json:"input_files"`
	OutputFiles []string  `json:"output_files,omitempty"`
	Errors      []string  `json:"errors,omitempty"`
}

// ConfigRequest 配置请求
type ConfigRequest struct {
	PandocPath   string `json:"pandoc_path,omitempty"`
	TemplateFile string `json:"template_file,omitempty"`
}

// ConfigResponse 配置响应
type ConfigResponse struct {
	Success      bool   `json:"success"`
	Message      string `json:"message"`
	PandocPath   string `json:"pandoc_path"`
	TemplateFile string `json:"template_file"`
	Error        string `json:"error,omitempty"`
}
