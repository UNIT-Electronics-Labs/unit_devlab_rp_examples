import { ssrRenderAttrs, ssrRenderStyle } from "vue/server-renderer";
import { useSSRContext } from "vue";
import { _ as _export_sfc } from "./plugin-vue_export-helper.1tPrXgE0.js";
const __pageData = JSON.parse('{"title":"13 Maquina Moore","description":"","frontmatter":{},"headers":[],"relativePath":"examples/digital-labs/13_maquina_moore.md","filePath":"examples/digital-labs/13_maquina_moore.md"}');
const _sfc_main = { name: "examples/digital-labs/13_maquina_moore.md" };
function _sfc_ssrRender(_ctx, _push, _parent, _attrs, $props, $setup, $data, $options) {
  _push(`<div${ssrRenderAttrs(_attrs)}><h1 id="_13-maquina-moore" tabindex="-1">13 Maquina Moore <a class="header-anchor" href="#_13-maquina-moore" aria-label="Permalink to &quot;13 Maquina Moore&quot;">​</a></h1><p>Practica de maquina de estados tipo Moore para detectar la secuencia <code>101</code>.</p><h2 id="objetivo" tabindex="-1">Objetivo <a class="header-anchor" href="#objetivo" aria-label="Permalink to &quot;Objetivo&quot;">​</a></h2><p>Implementar una FSM donde la salida depende solo del estado actual.</p><h2 id="verilog" tabindex="-1">Verilog <a class="header-anchor" href="#verilog" aria-label="Permalink to &quot;Verilog&quot;">​</a></h2><div class="language-bash vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">bash</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}">cd</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> digital-labs/13_maquina_moore</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> build</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> flash</span></span></code></pre></div><h2 id="vhdl" tabindex="-1">VHDL <a class="header-anchor" href="#vhdl" aria-label="Permalink to &quot;VHDL&quot;">​</a></h2><div class="language-bash vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">bash</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}">cd</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> digital-labs/13_maquina_moore</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> build</span><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}"> -c</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> devlab-vhdl.toml</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> flash</span></span></code></pre></div><h2 id="conceptos" tabindex="-1">Conceptos <a class="header-anchor" href="#conceptos" aria-label="Permalink to &quot;Conceptos&quot;">​</a></h2><ul><li>Estados codificados.</li><li>Transiciones por entrada.</li><li>Salida asociada al estado.</li></ul></div>`);
}
const _sfc_setup = _sfc_main.setup;
_sfc_main.setup = (props, ctx) => {
  const ssrContext = useSSRContext();
  (ssrContext.modules || (ssrContext.modules = /* @__PURE__ */ new Set())).add("examples/digital-labs/13_maquina_moore.md");
  return _sfc_setup ? _sfc_setup(props, ctx) : void 0;
};
const _13_maquina_moore = /* @__PURE__ */ _export_sfc(_sfc_main, [["ssrRender", _sfc_ssrRender]]);
export {
  __pageData,
  _13_maquina_moore as default
};
