🛡️ The "Defense Pro-Tip" (Bonus)

If an evaluator asks: "What happens if I try to send a 10 Gigabyte file? Does your server load all 10 GB into RAM before returning the 413 error?"

With the code above, the answer is technically yes, because the EventLoop reads the entire body before passing it to setHttpRequest.

If you want to be completely bulletproof, you can also check the Content-Length header while the EventLoop is still reading!

If you look at your EventLoop::appendToClientBuffer function, you already extract the Content-Length. You could easily add a check there:
C++

```
// Inside EventLoop::appendToClientBuffer, right after extracting Content-Length:
if (_clientContentLength[fd] > static_cast<size_t>(config.getClientMaxBodySize())) {
    // Stop reading immediately and trigger the 413 response!
}
```

However, for a standard 42 evaluation, checking it inside setHttpResponse (Step 1 & 2) is entirely sufficient to get the points on the grading sheet. Implement Steps 1 & 2, run a test with curl using a file larger than your limit, and watch your server successfully block it!