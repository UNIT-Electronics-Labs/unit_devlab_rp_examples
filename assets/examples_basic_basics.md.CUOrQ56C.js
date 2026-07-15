import{_ as i,o as a,c as n,a2 as p}from"./chunks/framework.HMsj64NP.js";const A=JSON.parse('{"title":"","description":"","frontmatter":{},"headers":[],"relativePath":"examples/basic/basics.md","filePath":"examples/basic/basics.md"}'),l={name:"examples/basic/basics.md"};function t(h,s,e,k,r,d){return a(),n("div",null,[...s[0]||(s[0]=[p(`<div class="language-C vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">C</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;">/**</span></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;"> * </span><span style="--shiki-light:#D73A49;--shiki-dark:#F97583;">@file</span><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;"> Practice_UART_05.c</span></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;"> * </span><span style="--shiki-light:#D73A49;--shiki-dark:#F97583;">@brief</span><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;"> Proyecto Practice_UART_05 para Raspberry Pi Pico</span></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;"> *</span></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;"> * </span><span style="--shiki-light:#D73A49;--shiki-dark:#F97583;">@author</span><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;"> obviousfancy</span></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;"> * </span><span style="--shiki-light:#D73A49;--shiki-dark:#F97583;">@date</span><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;"> 2026-07-10</span></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;"> *</span></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;"> * @board pico</span></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;"> * @sdk Raspberry Pi Pico SDK 2.2.0</span></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;"> */</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;">/* ─── Includes ─────────────────────────────────────────── */</span></span>
<span class="line"><span style="--shiki-light:#D73A49;--shiki-dark:#F97583;">#include</span><span style="--shiki-light:#032F62;--shiki-dark:#9ECBFF;"> &quot;pico/stdlib.h&quot;</span></span>
<span class="line"><span style="--shiki-light:#D73A49;--shiki-dark:#F97583;">#include</span><span style="--shiki-light:#032F62;--shiki-dark:#9ECBFF;"> &quot;hardware/uart.h&quot;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;">/* ─── Defines ──────────────────────────────────────────── */</span></span>
<span class="line"><span style="--shiki-light:#D73A49;--shiki-dark:#F97583;">#define</span><span style="--shiki-light:#6F42C1;--shiki-dark:#B392F0;"> UART_TX0</span><span style="--shiki-light:#005CC5;--shiki-dark:#79B8FF;"> 0</span></span>
<span class="line"><span style="--shiki-light:#D73A49;--shiki-dark:#F97583;">#define</span><span style="--shiki-light:#6F42C1;--shiki-dark:#B392F0;"> UART_RX0</span><span style="--shiki-light:#005CC5;--shiki-dark:#79B8FF;"> 1</span></span>
<span class="line"><span style="--shiki-light:#D73A49;--shiki-dark:#F97583;">#define</span><span style="--shiki-light:#6F42C1;--shiki-dark:#B392F0;"> UART_BAUD_RATE</span><span style="--shiki-light:#005CC5;--shiki-dark:#79B8FF;"> 115200</span></span>
<span class="line"><span style="--shiki-light:#D73A49;--shiki-dark:#F97583;">#define</span><span style="--shiki-light:#6F42C1;--shiki-dark:#B392F0;"> UART_ID</span><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;"> uart0</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;">/* ─── Main ─────────────────────────────────────────────── */</span></span>
<span class="line"><span style="--shiki-light:#D73A49;--shiki-dark:#F97583;">int</span><span style="--shiki-light:#6F42C1;--shiki-dark:#B392F0;"> main</span><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">() {</span></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;">    //stdio_init_all();</span></span>
<span class="line"><span style="--shiki-light:#6F42C1;--shiki-dark:#B392F0;">    uart_init</span><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">(UART_ID, UART_BAUD_RATE);</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#6F42C1;--shiki-dark:#B392F0;">    gpio_set_function</span><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">(UART_TX0, GPIO_FUNC_UART);</span></span>
<span class="line"><span style="--shiki-light:#6F42C1;--shiki-dark:#B392F0;">    gpio_set_function</span><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">(UART_RX0, GPIO_FUNC_UART);</span></span>
<span class="line"></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#D73A49;--shiki-dark:#F97583;">    while</span><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;"> (</span><span style="--shiki-light:#005CC5;--shiki-dark:#79B8FF;">1</span><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">) {</span></span>
<span class="line"><span style="--shiki-light:#6A737D;--shiki-dark:#6A737D;">        // TODO: lógica principal del proyecto</span></span>
<span class="line"><span style="--shiki-light:#6F42C1;--shiki-dark:#B392F0;">        uart_puts</span><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">(UART_ID, </span><span style="--shiki-light:#032F62;--shiki-dark:#9ECBFF;">&quot;Hello, UART!</span><span style="--shiki-light:#005CC5;--shiki-dark:#79B8FF;">\\n\\r</span><span style="--shiki-light:#032F62;--shiki-dark:#9ECBFF;">&quot;</span><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">);</span></span>
<span class="line"><span style="--shiki-light:#6F42C1;--shiki-dark:#B392F0;">        sleep_ms</span><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">(</span><span style="--shiki-light:#005CC5;--shiki-dark:#79B8FF;">1000</span><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">);</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">    }</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">}</span></span></code></pre></div><p>#include &quot;pico/stdlib.h&quot; #include &quot;hardware/uart.h&quot;</p><p>#define UART_ID uart0 #define BAUD_RATE 115200 #define UART_TX_PIN 0 #define UART_RX_PIN 1</p><p>int main() { uart_init(UART_ID, BAUD_RATE);</p><pre><code>gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

uart_puts(UART_ID, &quot;Eco UART listo. Escribe algo:\\r\\n&quot;);

while (true) {
    if (uart_is_readable(UART_ID)) {
        char c = uart_getc(UART_ID);
        uart_puts(UART_ID, &quot;Recibido: &quot;);
        uart_putc(UART_ID, c);
        uart_puts(UART_ID, &quot;\\r\\n&quot;);
    }
}
</code></pre><p>}</p>`,6)])])}const _=i(l,[["render",t]]);export{A as __pageData,_ as default};
