// API基础URL
const API_BASE = '/api';

// 显示标签页
function showTab(tabName) {
    // 隐藏所有标签页内容
    const contents = document.querySelectorAll('.tab-content');
    contents.forEach(content => content.classList.remove('active'));
    
    // 移除所有标签的active类
    const tabs = document.querySelectorAll('.tab');
    tabs.forEach(tab => tab.classList.remove('active'));
    
    // 显示选中的标签页
    document.getElementById(tabName).classList.add('active');
    event.target.classList.add('active');
}

// 显示状态信息
function showStatus(elementId, message, type = 'info') {
    const statusElement = document.getElementById(elementId);
    statusElement.textContent = message;
    statusElement.className = `status ${type}`;
    statusElement.style.display = 'block';
}

// 单文件转换
async function convertSingle() {
    const inputFile = document.getElementById('singleInputFile').files[0];
    if (!inputFile) {
        showStatus('singleStatus', '请选择一个Markdown文件', 'error');
        return;
    }

    const outputDir = document.getElementById('singleOutputDir').value.trim();
    const outputName = document.getElementById('singleOutputName').value.trim();

    const requestData = {
        input_file: inputFile.name, // 注意：这里只是文件名，实际应用中需要完整路径
        output_dir: outputDir || '',
        output_name: outputName || ''
    };

    try {
        showStatus('singleStatus', '正在转换...', 'info');
        
        const response = await fetch(`${API_BASE}/convert/single`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(requestData)
        });

        const result = await response.json();
        
        if (result.success) {
            showStatus('singleStatus', `转换成功！\n输出文件: ${result.output_file}`, 'success');
        } else {
            showStatus('singleStatus', `转换失败: ${result.error || result.message}`, 'error');
        }
    } catch (error) {
        showStatus('singleStatus', `请求失败: ${error.message}`, 'error');
    }
}

// 批量转换
async function convertBatch() {
    const inputFiles = document.getElementById('batchInputFiles').files;
    if (inputFiles.length === 0) {
        showStatus('batchStatus', '请选择至少一个Markdown文件', 'error');
        return;
    }

    const outputDir = document.getElementById('batchOutputDir').value.trim();
    const inputFileNames = Array.from(inputFiles).map(file => file.name);

    const requestData = {
        input_files: inputFileNames, // 注意：这里只是文件名，实际应用中需要完整路径
        output_dir: outputDir || ''
    };

    try {
        showStatus('batchStatus', '正在批量转换...', 'info');
        
        const response = await fetch(`${API_BASE}/convert/batch`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(requestData)
        });

        const result = await response.json();
        
        if (result.success) {
            let message = `${result.message}\n\n转换结果:\n`;
            result.results.forEach(item => {
                if (item.success) {
                    message += `✓ ${item.input_file} -> ${item.output_file}\n`;
                } else {
                    message += `✗ ${item.input_file}: ${item.error}\n`;
                }
            });
            showStatus('batchStatus', message, 'success');
        } else {
            showStatus('batchStatus', `批量转换失败: ${result.error || result.message}`, 'error');
        }
    } catch (error) {
        showStatus('batchStatus', `请求失败: ${error.message}`, 'error');
    }
}

// 加载配置
async function loadConfig() {
    try {
        showStatus('configStatus', '正在加载配置...', 'info');
        
        const response = await fetch(`${API_BASE}/config`);
        const result = await response.json();
        
        if (result.success) {
            document.getElementById('pandocPath').value = result.pandoc_path || '';
            document.getElementById('templateFile').value = result.template_file || '';
            showStatus('configStatus', '配置加载成功', 'success');
        } else {
            showStatus('configStatus', `加载配置失败: ${result.error || result.message}`, 'error');
        }
    } catch (error) {
        showStatus('configStatus', `请求失败: ${error.message}`, 'error');
    }
}

// 保存配置
async function saveConfig() {
    const pandocPath = document.getElementById('pandocPath').value.trim();
    const templateFile = document.getElementById('templateFile').value.trim();

    const requestData = {
        pandoc_path: pandocPath,
        template_file: templateFile
    };

    try {
        showStatus('configStatus', '正在保存配置...', 'info');
        
        const response = await fetch(`${API_BASE}/config`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(requestData)
        });

        const result = await response.json();
        
        if (result.success) {
            showStatus('configStatus', '配置保存成功', 'success');
        } else {
            showStatus('configStatus', `保存配置失败: ${result.error || result.message}`, 'error');
        }
    } catch (error) {
        showStatus('configStatus', `请求失败: ${error.message}`, 'error');
    }
}

// 验证配置
async function validateConfig() {
    try {
        showStatus('configStatus', '正在验证配置...', 'info');
        
        const response = await fetch(`${API_BASE}/config/validate`, {
            method: 'POST'
        });

        const result = await response.json();
        
        if (result.success) {
            showStatus('configStatus', '配置验证成功！所有设置都正确。', 'success');
        } else {
            showStatus('configStatus', `配置验证失败: ${result.error || result.message}`, 'error');
        }
    } catch (error) {
        showStatus('configStatus', `请求失败: ${error.message}`, 'error');
    }
}

// 页面加载完成后自动加载配置
document.addEventListener('DOMContentLoaded', function() {
    loadConfig();
});
