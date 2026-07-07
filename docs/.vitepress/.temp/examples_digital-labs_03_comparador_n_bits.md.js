import { ssrRenderAttrs, ssrRenderStyle } from "vue/server-renderer";
import { useSSRContext } from "vue";
import { _ as _export_sfc } from "./plugin-vue_export-helper.1tPrXgE0.js";
const __pageData = JSON.parse('{"title":"03 Comparador N bits","description":"","frontmatter":{},"headers":[],"relativePath":"examples/digital-labs/03_comparador_n_bits.md","filePath":"examples/digital-labs/03_comparador_n_bits.md"}');
const _sfc_main = { name: "examples/digital-labs/03_comparador_n_bits.md" };
function _sfc_ssrRender(_ctx, _push, _parent, _attrs, $props, $setup, $data, $options) {
  _push(`<div${ssrRenderAttrs(_attrs)}><h1 id="_03-comparador-n-bits" tabindex="-1">03 Comparador N bits <a class="header-anchor" href="#_03-comparador-n-bits" aria-label="Permalink to &quot;03 Comparador N bits&quot;">​</a></h1><p>Practica de comparacion binaria. Instancia <code>nbit_comparator</code>.</p><h2 id="objetivo" tabindex="-1">Objetivo <a class="header-anchor" href="#objetivo" aria-label="Permalink to &quot;Objetivo&quot;">​</a></h2><p>Comparar dos vectores y generar banderas como igualdad, mayor que o menor que.</p><h2 id="salida" tabindex="-1">Salida <a class="header-anchor" href="#salida" aria-label="Permalink to &quot;Salida&quot;">​</a></h2><ul><li><code>led_n</code>: encendido cuando <code>a == b</code>.</li></ul><h2 id="verilog" tabindex="-1">Verilog <a class="header-anchor" href="#verilog" aria-label="Permalink to &quot;Verilog&quot;">​</a></h2><div class="language-bash vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">bash</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}">cd</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> digital-labs/03_comparador_n_bits</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> build</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> flash</span></span></code></pre></div><h2 id="vhdl" tabindex="-1">VHDL <a class="header-anchor" href="#vhdl" aria-label="Permalink to &quot;VHDL&quot;">​</a></h2><div class="language-bash vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">bash</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}">cd</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> digital-labs/03_comparador_n_bits</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> build</span><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}"> -c</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> devlab-vhdl.toml</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> flash</span></span></code></pre></div><h2 id="archivos-clave" tabindex="-1">Archivos Clave <a class="header-anchor" href="#archivos-clave" aria-label="Permalink to &quot;Archivos Clave&quot;">​</a></h2><ul><li><code>src/digital_labs.v</code>: modulo <code>nbit_comparator</code>.</li><li><code>src/top.v</code>: top-level Verilog.</li><li><code>src/top.vhd</code>: version VHDL.</li></ul></div>`);
}
const _sfc_setup = _sfc_main.setup;
_sfc_main.setup = (props, ctx) => {
  const ssrContext = useSSRContext();
  (ssrContext.modules || (ssrContext.modules = /* @__PURE__ */ new Set())).add("examples/digital-labs/03_comparador_n_bits.md");
  return _sfc_setup ? _sfc_setup(props, ctx) : void 0;
};
const _03_comparador_n_bits = /* @__PURE__ */ _export_sfc(_sfc_main, [["ssrRender", _sfc_ssrRender]]);
export {
  __pageData,
  _03_comparador_n_bits as default
};
