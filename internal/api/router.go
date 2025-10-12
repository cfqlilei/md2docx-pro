package api

import (
	"net/http"

	"md2docx/internal/config"
)

// SetupRoutes 设置路由
func SetupRoutes(cfg *config.Config) *http.ServeMux {
	handler := New(cfg)
	mux := http.NewServeMux()

	// API路由
	mux.HandleFunc("/api/convert/single", corsMiddleware(handler.ConvertSingle))
	mux.HandleFunc("/api/convert/batch", corsMiddleware(handler.ConvertBatch))
	mux.HandleFunc("/api/config", corsMiddleware(configHandler(handler)))
	mux.HandleFunc("/api/config/validate", corsMiddleware(handler.ValidateConfig))
	mux.HandleFunc("/api/health", corsMiddleware(handler.Health))

	// 静态文件服务（用于前端）
	mux.Handle("/", http.FileServer(http.Dir("web/static/")))

	return mux
}

// configHandler 配置处理器，根据HTTP方法分发
func configHandler(h *Handler) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		switch r.Method {
		case http.MethodGet:
			h.GetConfig(w, r)
		case http.MethodPost:
			h.UpdateConfig(w, r)
		default:
			http.Error(w, "不支持的HTTP方法", http.StatusMethodNotAllowed)
		}
	}
}

// corsMiddleware CORS中间件
func corsMiddleware(next http.HandlerFunc) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		// 设置CORS头
		w.Header().Set("Access-Control-Allow-Origin", "*")
		w.Header().Set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS")
		w.Header().Set("Access-Control-Allow-Headers", "Content-Type, Authorization")

		// 处理预检请求
		if r.Method == http.MethodOptions {
			w.WriteHeader(http.StatusOK)
			return
		}

		// 调用下一个处理器
		next(w, r)
	}
}

// loggingMiddleware 日志中间件
func loggingMiddleware(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		// 这里可以添加日志记录逻辑
		next.ServeHTTP(w, r)
	})
}
