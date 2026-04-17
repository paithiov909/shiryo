function walk_blocks(blocks)
  local new_blocks = {}

  for _, block in ipairs(blocks) do
    if block.t == "CodeBlock" then
      table.insert(new_blocks, block)

      local classes = block.classes
      local text = block.text

      if classes:includes("glsl") then
        local meta_label = text:match("//|%s*label:%s*([%w%-%_%.]+)")
        if not meta_label then
          table.insert(new_blocks, pandoc.RawBlock("html", [[
<div class="callout-warning" title="P5 Filter Warning">
  <p>Missing or invalid label for shader code block. Expected format: <code>//| label: name.vert</code></p>
</div>]]))
        else
          local id, ext = meta_label:match("^(.-)%.(%a+)$")
          if ext == "vert" or ext == "frag" then
            local shader_type = ext == "vert" and "x-shader/x-vertex" or "x-shader/x-fragment"
            local script_tag = string.format(
              '<script id="%s" type="%s">\n%s\n</script>',
              id, shader_type, block.text
            )
            table.insert(new_blocks, pandoc.RawBlock("html", script_tag))
          else
            table.insert(new_blocks, pandoc.RawBlock("html", [[
<div class="callout-warning" title="P5 Filter Warning">
  <p>Unrecognized shader extension in label: must end in <code>.vert</code> or <code>.frag</code>.</p>
</div>]]))
          end
        end
      elseif classes:includes("js") then
        local script_tag = string.format('<script>\n%s\n</script>', block.text)
        table.insert(new_blocks, pandoc.RawBlock("html", script_tag))
      end

    elseif block.t == "Div" then
      local new_content = walk_blocks(block.content)
      block.content = new_content
      table.insert(new_blocks, block)

    else
      table.insert(new_blocks, block)
    end
  end

  return new_blocks
end

if quarto.doc.is_format("html:js") then
  function Pandoc(doc)
    quarto.doc.include_file("in-header", "header.html")
    local updated_blocks = walk_blocks(doc.blocks)
    return pandoc.Pandoc(updated_blocks, doc.meta)
  end
end
