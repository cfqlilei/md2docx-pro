#!/usr/bin/env node

/**
 * macOS完整应用启动脚本
 * 启动Go后端服务和Qt前端应用
 */

const { spawn, exec } = require("child_process");
const path = require("path");
const fs = require("fs");

// 项目根目录
const PROJECT_ROOT = path.resolve(__dirname, "..");

// 配置
const CONFIG = {
  backend: {
    binary: path.join(PROJECT_ROOT, "md2docx-server"),
    port: 8080,
  },
  frontend: {
    binary: path.join(
      PROJECT_ROOT,
      "qt-frontend",
      "build_md2docx_app",
      "build",
      "md2docx_app.app",
      "Contents",
      "MacOS",
      "md2docx_app"
    ),
  },
};

let backendProcess = null;
let frontendProcess = null;

// 清理函数
function cleanup() {
  console.log("\n正在关闭应用...");

  if (frontendProcess) {
    console.log("关闭前端应用...");
    frontendProcess.kill("SIGTERM");
  }

  if (backendProcess) {
    console.log("关闭后端服务...");
    backendProcess.kill("SIGTERM");
  }

  setTimeout(() => {
    process.exit(0);
  }, 2000);
}

// 注册清理处理器
process.on("SIGINT", cleanup);
process.on("SIGTERM", cleanup);
process.on("exit", cleanup);

// 检查文件是否存在
function checkFile(filePath, description) {
  if (!fs.existsSync(filePath)) {
    console.error(`错误: ${description} 不存在: ${filePath}`);
    console.error("请先构建项目");
    process.exit(1);
  }
}

// 等待后端服务启动
function waitForBackend(callback) {
  const http = require("http");

  function checkHealth() {
    const req = http.get(
      `http://localhost:${CONFIG.backend.port}/api/health`,
      (res) => {
        if (res.statusCode === 200) {
          console.log("✅ 后端服务已就绪");
          callback();
        } else {
          setTimeout(checkHealth, 1000);
        }
      }
    );

    req.on("error", () => {
      setTimeout(checkHealth, 1000);
    });
  }

  setTimeout(checkHealth, 2000);
}

// 启动后端服务
function startBackend() {
  return new Promise((resolve, reject) => {
    console.log("🚀 启动后端服务...");

    checkFile(CONFIG.backend.binary, "后端可执行文件");

    backendProcess = spawn(CONFIG.backend.binary, [], {
      cwd: PROJECT_ROOT,
      stdio: ["ignore", "pipe", "pipe"],
    });

    backendProcess.stdout.on("data", (data) => {
      console.log(`[后端] ${data.toString().trim()}`);
    });

    backendProcess.stderr.on("data", (data) => {
      console.error(`[后端错误] ${data.toString().trim()}`);
    });

    backendProcess.on("error", (error) => {
      console.error("后端服务启动失败:", error);
      reject(error);
    });

    backendProcess.on("exit", (code) => {
      console.log(`后端服务退出，代码: ${code}`);
    });

    // 等待后端服务就绪
    waitForBackend(resolve);
  });
}

// 启动前端应用
function startFrontend() {
  return new Promise((resolve, reject) => {
    console.log("🖥️  启动前端应用...");

    checkFile(CONFIG.frontend.binary, "前端可执行文件");

    frontendProcess = spawn(CONFIG.frontend.binary, [], {
      cwd: path.dirname(CONFIG.frontend.binary),
      stdio: ["ignore", "pipe", "pipe"],
    });

    frontendProcess.stdout.on("data", (data) => {
      console.log(`[前端] ${data.toString().trim()}`);
    });

    frontendProcess.stderr.on("data", (data) => {
      console.error(`[前端错误] ${data.toString().trim()}`);
    });

    frontendProcess.on("error", (error) => {
      console.error("前端应用启动失败:", error);
      reject(error);
    });

    frontendProcess.on("exit", (code) => {
      console.log(`前端应用退出，代码: ${code}`);
      cleanup();
    });

    resolve();
  });
}

// 主函数
async function main() {
  try {
    console.log("=== Markdown转Word工具 - 完整应用启动 ===");
    console.log(`项目目录: ${PROJECT_ROOT}`);
    console.log(`后端端口: ${CONFIG.backend.port}`);
    console.log("==========================================");

    // 启动后端服务
    await startBackend();

    // 启动前端应用
    await startFrontend();

    console.log("✅ 应用启动完成！");
    console.log("按 Ctrl+C 退出应用");
  } catch (error) {
    console.error("应用启动失败:", error);
    cleanup();
    process.exit(1);
  }
}

// 启动应用
main();
