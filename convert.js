async function convert(inputFile, outputHeaderFile) {
  const pcmData = new Uint8Array(await Bun.file(inputFile).arrayBuffer());

  let headerContent = "";
  headerContent += "#ifndef RAW_PCM_DATA_H\n";
  headerContent += "#define RAW_PCM_DATA_H\n\n";
  headerContent += "const uint8_t audio_pcm[] = {\n";

  pcmData.forEach((byte, i) => {
    if (i > 0 && i % 12 === 0) {
      headerContent += "\n";
    }
    headerContent += `0x${byte.toString(16).toUpperCase().padStart(2, "0")}, `;
  });

  headerContent += "\n};\n\n";
  headerContent += "#endif // RAW_PCM_DATA_H\n";

  await Bun.write(outputHeaderFile, headerContent);
  console.log(`header file written to ${outputHeaderFile}`);
}

convert("audio.pcm", "src/pcm_data.h");
