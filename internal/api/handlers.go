package api

import (
	"encoding/json"
	"fmt"
	"net/http"

	"md2docx/internal/config"
	"md2docx/internal/converter"
	"md2docx/internal/models"
)

// Handler API处理器
type Handler struct {
	converter *converter.Converter
	config    *config.Config
}

// New 创建新的API处理器
func New(cfg *config.Config) *Handler {
	return &Handler{
		converter: converter.New(cfg),
		config:    cfg,
	}
}

// ConvertSingle 单文件转换接口
func (h *Handler) ConvertSingle(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "只支持POST方法", http.StatusMethodNotAllowed)
		return
	}

	var req models.ConversionRequest
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		h.sendErrorResponse(w, "请求参数解析失败", err, http.StatusBadRequest)
		return
	}

	// 执行转换
	response, err := h.converter.ConvertSingle(&req)
	if err != nil {
		h.sendErrorResponse(w, "转换服务内部错误", err, http.StatusInternalServerError)
		return
	}

	h.sendJSONResponse(w, response, http.StatusOK)
}

// ConvertBatch 批量转换接口
func (h *Handler) ConvertBatch(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "只支持POST方法", http.StatusMethodNotAllowed)
		return
	}

	var req models.BatchConversionRequest
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		h.sendErrorResponse(w, "请求参数解析失败", err, http.StatusBadRequest)
		return
	}

	// 执行批量转换
	response, err := h.converter.ConvertBatch(&req)
	if err != nil {
		h.sendErrorResponse(w, "转换服务内部错误", err, http.StatusInternalServerError)
		return
	}

	h.sendJSONResponse(w, response, http.StatusOK)
}

// GetConfig 获取配置接口
func (h *Handler) GetConfig(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		http.Error(w, "只支持GET方法", http.StatusMethodNotAllowed)
		return
	}

	response := &models.ConfigResponse{
		Success:      true,
		Message:      "获取配置成功",
		PandocPath:   h.config.PandocPath,
		TemplateFile: h.config.TemplateFile,
	}

	h.sendJSONResponse(w, response, http.StatusOK)
}

// UpdateConfig 更新配置接口
func (h *Handler) UpdateConfig(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "只支持POST方法", http.StatusMethodNotAllowed)
		return
	}

	var req models.ConfigRequest
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		h.sendErrorResponse(w, "请求参数解析失败", err, http.StatusBadRequest)
		return
	}

	// 更新配置
	if err := h.config.Update(req.PandocPath, req.TemplateFile); err != nil {
		h.sendErrorResponse(w, "配置更新失败", err, http.StatusBadRequest)
		return
	}

	// 更新转换器配置
	h.converter.UpdateConfig(h.config)

	response := &models.ConfigResponse{
		Success:      true,
		Message:      "配置更新成功",
		PandocPath:   h.config.PandocPath,
		TemplateFile: h.config.TemplateFile,
	}

	h.sendJSONResponse(w, response, http.StatusOK)
}

// ValidateConfig 验证配置接口
func (h *Handler) ValidateConfig(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "只支持POST方法", http.StatusMethodNotAllowed)
		return
	}

	response := &models.ConfigResponse{
		Success:      true,
		Message:      "配置验证成功",
		PandocPath:   h.config.PandocPath,
		TemplateFile: h.config.TemplateFile,
	}

	// 验证Pandoc
	if err := h.config.ValidatePandoc(); err != nil {
		response.Success = false
		response.Error = fmt.Sprintf("Pandoc配置无效: %v", err)
		h.sendJSONResponse(w, response, http.StatusOK)
		return
	}

	// 验证模板文件
	if err := h.config.ValidateTemplate(); err != nil {
		response.Success = false
		response.Error = fmt.Sprintf("模板文件配置无效: %v", err)
		h.sendJSONResponse(w, response, http.StatusOK)
		return
	}

	h.sendJSONResponse(w, response, http.StatusOK)
}

// Health 健康检查接口
func (h *Handler) Health(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		http.Error(w, "只支持GET方法", http.StatusMethodNotAllowed)
		return
	}

	response := map[string]interface{}{
		"status":  "ok",
		"message": "服务运行正常",
	}

	// 检查Pandoc是否可用
	if err := h.config.ValidatePandoc(); err != nil {
		response["pandoc_status"] = "error"
		response["pandoc_error"] = err.Error()
	} else {
		response["pandoc_status"] = "ok"
	}

	h.sendJSONResponse(w, response, http.StatusOK)
}

// sendJSONResponse 发送JSON响应
func (h *Handler) sendJSONResponse(w http.ResponseWriter, data interface{}, statusCode int) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(statusCode)
	json.NewEncoder(w).Encode(data)
}

// sendErrorResponse 发送错误响应
func (h *Handler) sendErrorResponse(w http.ResponseWriter, message string, err error, statusCode int) {
	response := map[string]interface{}{
		"success": false,
		"message": message,
		"error":   err.Error(),
	}
	h.sendJSONResponse(w, response, statusCode)
}
