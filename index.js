function toggle_expandable(caller){
    var content = caller.nextElementSibling;
    caller.classList.toggle("active-expandable-block-button");
    if(content.style.maxHeight){
        content.style.maxHeight = null;
    }else{
        content.style.maxHeight = content.scrollHeight + "px";
    }
//    if(content.style.display === "block"){
//        content.style.display = "none"
//    }else{
//        content.style.display = "block"
//    }
}
function select_choice_answer(caller){
    caller.parentElement.querySelectorAll('.choice-answer-correct, .choice-answer-wrong').forEach(s=>s.classList.remove("active-choice-answer"))
    caller.classList.add("active-choice-answer")
    ready = caller.parentElement.querySelector('.choice-ready-button')
    ready.classList.add('choice-ready-button-active')
}
function select_multi_choice_answer(caller){
    caller.classList.toggle("active-multi-choice-answer")
    ready = caller.parentElement.querySelector('.multi-choice-ready-button')
    ready.classList.add('multi-choice-ready-button-active')
}
const NEXT_MSG = "Next question"
function ready_multi_choice_answer(caller){
    question_div = caller.parentElement.parentElement
    next_question_div = question_div.nextElementSibling
    if(caller.innerText == NEXT_MSG){
        question_div.style.display = "none"
        next_question_div.style.display = "block"
    }else{
        caller.parentElement.querySelectorAll('.multi-choice-explanation-wrong, .multi-choice-explanation-correct').forEach(s=>
            s.style.maxHeight = s.scrollHeight + "px"
        )
        caller.parentElement.querySelectorAll('.multi-choice-answer-wrong').forEach(s=> {
            s.style.backgroundColor = 'red';
            s.classList.remove('active-multi-choice-answer');
        })
        caller.parentElement.querySelectorAll('.multi-choice-answer-correct').forEach(s=>{
            s.style.backgroundColor = 'green';
            s.classList.remove('active-multi-choice-answer');
        })
        if(next_question_div){
            caller.innerText = NEXT_MSG
        }else{
            caller.style.display = 'none'
        }
    }
}
function ready_choice_answer(caller){
    question_div = caller.parentElement.parentElement
    next_question_div = question_div.nextElementSibling
    if(caller.innerText == NEXT_MSG){
        question_div.style.display = "none"
        next_question_div.style.display = "block"
    }else if(caller.classList.contains('choice-ready-button-active')){
        caller.parentElement.querySelectorAll('.choice-explanation-wrong, .choice-explanation-correct').forEach(s=>
            s.style.maxHeight = s.scrollHeight + "px"
        )
        caller.parentElement.querySelectorAll('.choice-answer-wrong').forEach(s=> {
            s.style.backgroundColor = 'red';
            s.classList.remove('active-choice-answer');
        })
        caller.parentElement.querySelectorAll('.choice-answer-correct').forEach(s=>{
            s.style.backgroundColor = 'green';
            s.classList.remove('active-choice-answer');
        })
        if(next_question_div){
            caller.innerText = NEXT_MSG
        }else{
            caller.style.display = 'none'
        }
    }
}
function select_multichoice_answer(caller){
    caller.classList.toggle("active-multi-choice-answer")
}
function after_load(){
    hljs.highlightAll()
}