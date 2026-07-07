import { ssrRenderAttrs, ssrRenderStyle } from "vue/server-renderer";
import { useSSRContext } from "vue";
import { _ as _export_sfc } from "./plugin-vue_export-helper.1tPrXgE0.js";
const __pageData = JSON.parse('{"title":"Digital Labs","description":"","frontmatter":{},"headers":[],"relativePath":"examples/digital-labs/index.md","filePath":"examples/digital-labs/index.md"}');
const _sfc_main = { name: "examples/digital-labs/index.md" };
function _sfc_ssrRender(_ctx, _push, _parent, _attrs, $props, $setup, $data, $options) {
  _push(`<div${ssrRenderAttrs(_attrs)}><h1 id="digital-labs" tabindex="-1">Digital Labs <a class="header-anchor" href="#digital-labs" aria-label="Permalink to &quot;Digital Labs&quot;">​</a></h1><p>Las practicas de <code>digital-labs</code> cubren bloques combinacionales, contadores y maquinas de estado. Todas siguen la misma estructura:</p><div class="language-text vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">text</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span>digital-labs/&lt;practica&gt;/</span></span>
<span class="line"><span>|-- devlab.toml</span></span>
<span class="line"><span>|-- devlab-vhdl.toml</span></span>
<span class="line"><span>|-- pins.cst</span></span>
<span class="line"><span>|-- src/</span></span>
<span class="line"><span>|   |-- digital_labs.v</span></span>
<span class="line"><span>|   |-- top.v</span></span>
<span class="line"><span>|   \`-- top.vhd</span></span>
<span class="line"><span>\`-- README.md</span></span></code></pre></div><h2 id="practicas-combinacionales" tabindex="-1">Practicas Combinacionales <a class="header-anchor" href="#practicas-combinacionales" aria-label="Permalink to &quot;Practicas Combinacionales&quot;">​</a></h2><ul><li><a href="./01_sumador_n_bits">01 Sumador N bits</a></li><li><a href="./02_restador_n_bits">02 Restador N bits</a></li><li><a href="./03_comparador_n_bits">03 Comparador N bits</a></li><li><a href="./04_decodificador_7_segmentos">04 Decodificador 7 segmentos</a></li><li><a href="./05_decodificador_16_segmentos">05 Decodificador 16 segmentos</a></li><li><a href="./06_restador_a_7_segmentos">06 Restador a 7 segmentos</a></li><li><a href="./07_alu_n_bits">07 ALU N bits</a></li></ul><h2 id="practicas-secuenciales" tabindex="-1">Practicas Secuenciales <a class="header-anchor" href="#practicas-secuenciales" aria-label="Permalink to &quot;Practicas Secuenciales&quot;">​</a></h2><ul><li><a href="./08_senal_reloj">08 Senal de reloj</a></li><li><a href="./09_contador_ascendente">09 Contador ascendente</a></li><li><a href="./10_contador_descendente">10 Contador descendente</a></li><li><a href="./11_contador_ascendente_descendente">11 Contador ascendente-descendente</a></li><li><a href="./12_contador_arranque_paro_reset">12 Contador con arranque, paro y reset</a></li><li><a href="./13_maquina_moore">13 Maquina Moore</a></li><li><a href="./14_maquina_mealy">14 Maquina Mealy</a></li></ul><h2 id="comandos-base" tabindex="-1">Comandos Base <a class="header-anchor" href="#comandos-base" aria-label="Permalink to &quot;Comandos Base&quot;">​</a></h2><div class="language-bash vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">bash</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}">cd</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> digital-labs/01_sumador_n_bits</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> build</span></span>
<span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> flash</span></span></code></pre></div><p>Para VHDL:</p><div class="language-bash vp-adaptive-theme"><button title="Copy Code" class="copy"></button><span class="lang">bash</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="${ssrRenderStyle({ "--shiki-light": "#6F42C1", "--shiki-dark": "#B392F0" })}">devlab</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> build</span><span style="${ssrRenderStyle({ "--shiki-light": "#005CC5", "--shiki-dark": "#79B8FF" })}"> -c</span><span style="${ssrRenderStyle({ "--shiki-light": "#032F62", "--shiki-dark": "#9ECBFF" })}"> devlab-vhdl.toml</span></span></code></pre></div></div>`);
}
const _sfc_setup = _sfc_main.setup;
_sfc_main.setup = (props, ctx) => {
  const ssrContext = useSSRContext();
  (ssrContext.modules || (ssrContext.modules = /* @__PURE__ */ new Set())).add("examples/digital-labs/index.md");
  return _sfc_setup ? _sfc_setup(props, ctx) : void 0;
};
const index = /* @__PURE__ */ _export_sfc(_sfc_main, [["ssrRender", _sfc_ssrRender]]);
export {
  __pageData,
  index as default
};
