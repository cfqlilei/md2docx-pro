package tests

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"time"
)

// TestResult 测试结果
type TestResult struct {
	Package string
	Success bool
	Output  string
	Error   string
}

func main() {
	fmt.Println("=== Markdown转Word工具 - 自动化测试 ===")
	fmt.Printf("开始时间: %s\n", time.Now().Format("2006-01-02 15:04:05"))
	fmt.Println("==========================================")

	// 获取项目根目录
	rootDir, err := os.Getwd()
	if err != nil {
		fmt.Printf("获取当前目录失败: %v\n", err)
		os.Exit(1)
	}

	// 如果当前在tests目录，则回到上级目录
	if filepath.Base(rootDir) == "tests" {
		rootDir = filepath.Dir(rootDir)
	}

	// 切换到项目根目录
	if err := os.Chdir(rootDir); err != nil {
		fmt.Printf("切换到项目根目录失败: %v\n", err)
		os.Exit(1)
	}

	fmt.Printf("项目根目录: %s\n", rootDir)
	fmt.Println()

	// 运行测试
	results := []TestResult{}

	// 1. 运行单元测试
	fmt.Println("1. 运行单元测试...")
	unitTestPackages := []string{
		"./internal/config",
		"./internal/converter",
		"./internal/api",
		"./pkg/utils",
	}

	for _, pkg := range unitTestPackages {
		result := runTest(pkg, false)
		results = append(results, result)
		printTestResult(result)
	}

	// 2. 运行集成测试（如果环境支持）
	fmt.Println("\n2. 运行集成测试...")
	if checkPandocAvailable() {
		fmt.Println("检测到Pandoc，运行集成测试...")
		os.Setenv("RUN_INTEGRATION_TESTS", "1")
		result := runTest("./tests", false)
		results = append(results, result)
		printTestResult(result)
	} else {
		fmt.Println("未检测到Pandoc，跳过集成测试")
		results = append(results, TestResult{
			Package: "./tests",
			Success: false,
			Output:  "跳过：未安装Pandoc",
			Error:   "Pandoc未安装",
		})
	}

	// 3. 生成测试覆盖率报告
	fmt.Println("\n3. 生成测试覆盖率报告...")
	coverageResult := runCoverageTest()
	results = append(results, coverageResult)
	printTestResult(coverageResult)

	// 4. 运行基准测试（可选）
	if os.Getenv("RUN_BENCHMARK_TESTS") == "1" {
		fmt.Println("\n4. 运行基准测试...")
		benchmarkResult := runBenchmarkTest()
		results = append(results, benchmarkResult)
		printTestResult(benchmarkResult)
	}

	// 生成测试报告
	generateTestReport(results)

	// 输出总结
	fmt.Println("\n==========================================")
	fmt.Println("测试总结:")

	totalTests := len(results)
	passedTests := 0
	for _, result := range results {
		if result.Success {
			passedTests++
		}
	}

	fmt.Printf("总测试包数: %d\n", totalTests)
	fmt.Printf("通过: %d\n", passedTests)
	fmt.Printf("失败: %d\n", totalTests-passedTests)
	fmt.Printf("成功率: %.1f%%\n", float64(passedTests)/float64(totalTests)*100)
	fmt.Printf("结束时间: %s\n", time.Now().Format("2006-01-02 15:04:05"))

	if passedTests == totalTests {
		fmt.Println("🎉 所有测试通过！")
		os.Exit(0)
	} else {
		fmt.Println("❌ 部分测试失败")
		os.Exit(1)
	}
}

// runTest 运行指定包的测试
func runTest(pkg string, verbose bool) TestResult {
	args := []string{"test"}
	if verbose {
		args = append(args, "-v")
	}
	args = append(args, pkg)

	cmd := exec.Command("go", args...)
	output, err := cmd.CombinedOutput()

	result := TestResult{
		Package: pkg,
		Success: err == nil,
		Output:  string(output),
	}

	if err != nil {
		result.Error = err.Error()
	}

	return result
}

// runCoverageTest 运行覆盖率测试
func runCoverageTest() TestResult {
	cmd := exec.Command("go", "test", "-cover", "./...")
	output, err := cmd.CombinedOutput()

	result := TestResult{
		Package: "coverage",
		Success: err == nil,
		Output:  string(output),
	}

	if err != nil {
		result.Error = err.Error()
	}

	return result
}

// runBenchmarkTest 运行基准测试
func runBenchmarkTest() TestResult {
	os.Setenv("RUN_BENCHMARK_TESTS", "1")
	cmd := exec.Command("go", "test", "-bench=.", "./tests")
	output, err := cmd.CombinedOutput()

	result := TestResult{
		Package: "benchmark",
		Success: err == nil,
		Output:  string(output),
	}

	if err != nil {
		result.Error = err.Error()
	}

	return result
}

// checkPandocAvailable 检查Pandoc是否可用
func checkPandocAvailable() bool {
	cmd := exec.Command("pandoc", "--version")
	return cmd.Run() == nil
}

// printTestResult 打印测试结果
func printTestResult(result TestResult) {
	status := "❌ 失败"
	if result.Success {
		status = "✅ 通过"
	}

	fmt.Printf("  %s %s\n", status, result.Package)

	if !result.Success && result.Error != "" {
		fmt.Printf("    错误: %s\n", result.Error)
	}

	// 如果输出包含有用信息，显示部分输出
	if result.Output != "" {
		lines := strings.Split(result.Output, "\n")
		for _, line := range lines {
			if strings.Contains(line, "PASS") || strings.Contains(line, "FAIL") ||
				strings.Contains(line, "coverage:") || strings.Contains(line, "ok") {
				fmt.Printf("    %s\n", line)
			}
		}
	}
}

// generateTestReport 生成测试报告
func generateTestReport(results []TestResult) {
	reportDir := "tests/results"
	if err := os.MkdirAll(reportDir, 0755); err != nil {
		fmt.Printf("创建报告目录失败: %v\n", err)
		return
	}

	reportFile := filepath.Join(reportDir, fmt.Sprintf("test_report_%s.txt",
		time.Now().Format("20060102_150405")))

	file, err := os.Create(reportFile)
	if err != nil {
		fmt.Printf("创建报告文件失败: %v\n", err)
		return
	}
	defer file.Close()

	// 写入报告头部
	file.WriteString("Markdown转Word工具 - 测试报告\n")
	file.WriteString("================================\n")
	file.WriteString(fmt.Sprintf("生成时间: %s\n\n", time.Now().Format("2006-01-02 15:04:05")))

	// 写入测试结果
	for _, result := range results {
		status := "失败"
		if result.Success {
			status = "通过"
		}

		file.WriteString(fmt.Sprintf("包: %s\n", result.Package))
		file.WriteString(fmt.Sprintf("状态: %s\n", status))

		if result.Error != "" {
			file.WriteString(fmt.Sprintf("错误: %s\n", result.Error))
		}

		file.WriteString("输出:\n")
		file.WriteString(result.Output)
		file.WriteString("\n" + strings.Repeat("-", 50) + "\n\n")
	}

	fmt.Printf("测试报告已生成: %s\n", reportFile)
}
