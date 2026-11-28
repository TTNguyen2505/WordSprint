import assert from "node:assert/strict";
import { after, test } from "node:test";
import React from "react";
import { renderToStaticMarkup } from "react-dom/server";
import { createServer } from "vite";

const server = await createServer({
    server: { middlewareMode: true },
    appType: "custom"
});
after(() => server.close());
const { default: Quiz } = await server.ssrLoadModule(
    "/src/components/Quiz.jsx"
);

test("audio questions show their hint image, blank slots and sound controls", () => {
    const markup = renderToStaticMarkup(
        React.createElement(Quiz, {
            mode: "audio",
            imageSrc: "clue.png",
            letters: ["", "", "", " ", "", ""]
        })
    );
    assert.match(markup, /<img\b[^>]*src="clue\.png"/);
    assert.match(markup, /PLAY SOUND/);
    assert.equal((markup.match(/>_<\/span>/g) ?? []).length, 5);
    assert.match(markup, /aria-label="Space"/);
});

test("incorrect answers show accessible supporting text", () => {
    const markup = renderToStaticMarkup(
        React.createElement(Quiz, {
            answer: "WRONG",
            answerError: "Incorrect answer. Try again."
        })
    );
    assert.match(markup, /aria-invalid="true"/);
    assert.match(markup, /aria-describedby="[^"]+-supporting-text"/);
    assert.match(markup, /Incorrect answer\. Try again\./);
});
