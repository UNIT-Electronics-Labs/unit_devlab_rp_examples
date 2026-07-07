import { ssrRenderAttrs, ssrRenderStyle } from "vue/server-renderer";
import { useSSRContext } from "vue";
import { _ as _export_sfc } from "./plugin-vue_export-helper.1tPrXgE0.js";
const __pageData = JSON.parse('{"title":"09 Contador Ascendente","description":"","frontmatter":{},"headers":[],"relativePath":"examples/digital-labs/09_contador_ascendente.md","filePath":"examples/digital-labs/09_contador_ascendente.md"}');
const _sfc_main = { name: "examples/digital-labs/09_contador_ascendente.md" };
function _sfc_ssrRender(_ctx, _push, _parent, _attrs, $props, $setup, $data, $options) {
  _push(`<div${ssrRenderAttrs(_attrs)}><h1 id="_09-contador-ascendente" tabindex="-1">09 Contador Ascendente <a class="header-anchor" href="#_09-contador-ascendente" aria-label="Permalink to &quot;09 Contador Ascendente&quot;">​</a></h1><p>Practica de contador que incrementa con el reloj.</p><h2 id="objetivo" tabindex="-1">Objetivo <a class="header-anchor" href="#objetivo" aria-label="Permalink to &quot;Objetivo&quot;">​</a></h2><p>Implementar estado secuencial y observar como cambia con cada ciclo de reloj.</p><h2 id="verilog" tabindex="-1">Verilog <a class="header-anchor" href="#verilog" aria-label="Permalink to &quot;Verilog&quot;">​</a></h2><div class="language-bash vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">bash</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}">cd</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> digital-labs/09_contador_ascendente</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> build</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> flash</span></span></code></pre></div><h2 id="vhdl" tabindex="-1">VHDL <a class="header-anchor" href="#vhdl" aria-label="Permalink to &quot;VHDL&quot;">​</a></h2><div class="language-bash vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">bash</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}">cd</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> digital-labs/09_contador_ascendente</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> build</span><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}"> -c</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> devlab-vhdl.toml</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> flash</span></span></code></pre></div><h2 id="conceptos" tabindex="-1">Conceptos <a class="header-anchor" href="#conceptos" aria-label="Permalink to &quot;Conceptos&quot;">​</a></h2><ul><li>Registro de estado.</li><li>Incremento en flanco de subida.</li><li>Reset activo en bajo cuando aplica.</li></ul></div>`);
}
const _sfc_setup = _sfc_main.setup;
_sfc_main.setup = (props, ctx) => {
  const ssrContext = useSSRContext();
  (ssrContext.modules || (ssrContext.modules = /* @__PURE__ */ new Set())).add("examples/digital-labs/09_contador_ascendente.md");
  return _sfc_setup ? _sfc_setup(props, ctx) : void 0;
};
const _09_contador_ascendente = /* @__PURE__ */ _export_sfc(_sfc_main, [["ssrRender", _sfc_ssrRender]]);
export {
  __pageData,
  _09_contador_ascendente as default
};
